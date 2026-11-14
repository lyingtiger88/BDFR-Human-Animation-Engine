#include "bdfr/performance/SessionCodec.h"
#include "bdfr/performance/Validation.h"

#include <bit>
#include <limits>
#include <stdexcept>
#include <type_traits>

namespace bdfr::performance {
namespace {
constexpr std::uint8_t magic[8] = {'B','D','F','R','S','E','S','1'};
constexpr std::uint32_t max_items = 2'000'000;
constexpr std::uint32_t max_string = 16'000'000;

class Writer {
public:
    std::vector<std::uint8_t> data;
    void u8(std::uint8_t v) { data.push_back(v); }
    void u32(std::uint32_t v) { for (int i=0;i<4;++i) u8(static_cast<std::uint8_t>(v >> (i*8))); }
    void i32(std::int32_t v) { u32(std::bit_cast<std::uint32_t>(v)); }
    void i64(std::int64_t v) {
        const auto n=std::bit_cast<std::uint64_t>(v);
        for(int i=0;i<8;++i) u8(static_cast<std::uint8_t>(n >> (i*8)));
    }
    void f32(float v) { u32(std::bit_cast<std::uint32_t>(v)); }
    void str(const std::string& s) {
        if(s.size()>max_string) throw std::runtime_error("String exceeds archive limit");
        u32(static_cast<std::uint32_t>(s.size()));
        data.insert(data.end(),s.begin(),s.end());
    }
    void count(std::size_t n) {
        if(n>max_items) throw std::runtime_error("Collection exceeds archive limit");
        u32(static_cast<std::uint32_t>(n));
    }
    void vec(const Vec3f& v){f32(v.x);f32(v.y);f32(v.z);}
    void quat(const Quaternionf& q){f32(q.x);f32(q.y);f32(q.z);f32(q.w);}
    void header(const SampleHeader& h){
        i64(h.source_time_ns);i64(h.session_time_ns);f32(h.confidence);
        u8(h.valid?1:0);u8(static_cast<std::uint8_t>(h.space));str(h.source_id);
    }
    void channels(const std::vector<ScalarChannel>& v){
        count(v.size());for(const auto& x:v){str(x.name);f32(x.value);f32(x.confidence);}
    }
    void joint(const JointSample& j){
        str(j.name);i32(j.parent_index);vec(j.translation);quat(j.rotation);f32(j.confidence);
    }
    void hand(const HandSample& h){
        header(h.header);for(const auto& j:h.joints) joint(j);
        channels(h.channels);u8(h.has_palm_pose?1:0);vec(h.palm_position);quat(h.palm_rotation);
    }
};

class Reader {
public:
    explicit Reader(std::span<const std::uint8_t> b):bytes(b){}
    std::span<const std::uint8_t> bytes;std::size_t pos{0};
    void need(std::size_t n){if(n>bytes.size()-pos) throw std::runtime_error("Truncated session archive");}
    std::uint8_t u8(){need(1);return bytes[pos++];}
    std::uint32_t u32(){std::uint32_t v=0;for(int i=0;i<4;++i)v|=static_cast<std::uint32_t>(u8())<<(i*8);return v;}
    std::int32_t i32(){return std::bit_cast<std::int32_t>(u32());}
    std::int64_t i64(){std::uint64_t v=0;for(int i=0;i<8;++i)v|=static_cast<std::uint64_t>(u8())<<(i*8);return std::bit_cast<std::int64_t>(v);}
    float f32(){return std::bit_cast<float>(u32());}
    std::string str(){
        const auto n=u32();if(n>max_string)throw std::runtime_error("String exceeds archive limit");
        need(n);std::string s(reinterpret_cast<const char*>(bytes.data()+pos),n);pos+=n;return s;
    }
    std::uint32_t count(){const auto n=u32();if(n>max_items)throw std::runtime_error("Collection exceeds archive limit");return n;}
    Vec3f vec(){return {f32(),f32(),f32()};}
    Quaternionf quat(){return {f32(),f32(),f32(),f32()};}
    SampleHeader header(){
        SampleHeader h;h.source_time_ns=i64();h.session_time_ns=i64();h.confidence=f32();
        const auto valid=u8(),space=u8();
        if(valid>1||space>static_cast<std::uint8_t>(CoordinateSpace::LocalJoint))
            throw std::runtime_error("Invalid sample header enum");
        h.valid=valid!=0;h.space=static_cast<CoordinateSpace>(space);h.source_id=str();return h;
    }
    std::vector<ScalarChannel> channels(){
        std::vector<ScalarChannel> v;v.reserve(count());
        // Reserve count once via the loop's fixed upper bound below.
        return v;
    }
    std::vector<ScalarChannel> read_channels(){
        const auto n=count();std::vector<ScalarChannel> v;v.reserve(n);
        for(std::uint32_t i=0;i<n;++i)v.push_back({str(),f32(),f32()});return v;
    }
    JointSample joint(){JointSample j;j.name=str();j.parent_index=i32();j.translation=vec();j.rotation=quat();j.confidence=f32();return j;}
    HandSample hand(){
        HandSample h;h.header=header();for(auto& j:h.joints)j=joint();
        h.channels=read_channels();const auto palm=u8();if(palm>1)throw std::runtime_error("Invalid palm flag");
        h.has_palm_pose=palm!=0;h.palm_position=vec();h.palm_rotation=quat();return h;
    }
};

void write_frame(Writer& w,const PerformanceFrame& f){
    w.i64(f.session_time_ns);
    w.count(f.faces.size());for(const auto& x:f.faces){
        w.header(x.header);w.count(x.curves.size());for(const auto& c:x.curves){w.str(c.name);w.f32(c.value);}
        w.vec(x.head_translation);w.quat(x.head_rotation);w.vec(x.gaze_direction);
    }
    w.count(f.heads.size());for(const auto& x:f.heads){w.header(x.header);w.vec(x.translation);w.quat(x.rotation);w.channels(x.channels);}
    w.count(f.gaze.size());for(const auto& x:f.gaze){w.header(x.header);w.channels(x.channels);}
    w.count(f.bodies.size());for(const auto& x:f.bodies){
        w.header(x.header);w.str(x.skeleton_profile);w.count(x.joints.size());for(const auto& j:x.joints)w.joint(j);
        w.channels(x.channels);w.vec(x.root_translation);w.quat(x.root_rotation);
    }
    w.count(f.retargeted_bodies.size());for(const auto& x:f.retargeted_bodies){
        w.header(x.header);w.str(x.profile_name);w.vec(x.root_translation);w.quat(x.root_rotation);
        w.count(x.bones.size());for(const auto& b:x.bones){w.str(b.target_bone);w.quat(b.rotation);}
    }
    w.count(f.left_hands.size());for(const auto& x:f.left_hands)w.hand(x);
    w.count(f.right_hands.size());for(const auto& x:f.right_hands)w.hand(x);
    w.count(f.speech.size());for(const auto& x:f.speech){
        w.header(x.header);w.str(x.text);w.str(x.phoneme);w.str(x.viseme);w.f32(x.normalized_energy);w.f32(x.pitch_hz);
    }
    w.count(f.emotions.size());for(const auto& x:f.emotions){w.header(x.header);w.str(x.label);w.f32(x.intensity);}
}
PerformanceFrame read_frame(Reader& r){
    PerformanceFrame f;f.session_time_ns=r.i64();
    auto n=r.count();f.faces.reserve(n);for(std::uint32_t i=0;i<n;++i){
        FaceSample x;x.header=r.header();auto m=r.count();x.curves.reserve(m);
        for(std::uint32_t k=0;k<m;++k)x.curves.push_back({r.str(),r.f32()});
        x.head_translation=r.vec();x.head_rotation=r.quat();x.gaze_direction=r.vec();f.faces.push_back(std::move(x));
    }
    n=r.count();f.heads.reserve(n);for(std::uint32_t i=0;i<n;++i){HeadSample x;x.header=r.header();x.translation=r.vec();x.rotation=r.quat();x.channels=r.read_channels();f.heads.push_back(std::move(x));}
    n=r.count();f.gaze.reserve(n);for(std::uint32_t i=0;i<n;++i){GazeSample x;x.header=r.header();x.channels=r.read_channels();f.gaze.push_back(std::move(x));}
    n=r.count();f.bodies.reserve(n);for(std::uint32_t i=0;i<n;++i){
        SkeletonSample x;x.header=r.header();x.skeleton_profile=r.str();auto m=r.count();x.joints.reserve(m);
        for(std::uint32_t k=0;k<m;++k)x.joints.push_back(r.joint());
        x.channels=r.read_channels();x.root_translation=r.vec();x.root_rotation=r.quat();f.bodies.push_back(std::move(x));
    }
    n=r.count();f.retargeted_bodies.reserve(n);for(std::uint32_t i=0;i<n;++i){
        RetargetedSkeletonSample x;x.header=r.header();x.profile_name=r.str();x.root_translation=r.vec();x.root_rotation=r.quat();
        auto m=r.count();x.bones.reserve(m);for(std::uint32_t k=0;k<m;++k)x.bones.push_back({r.str(),r.quat()});f.retargeted_bodies.push_back(std::move(x));
    }
    n=r.count();f.left_hands.reserve(n);for(std::uint32_t i=0;i<n;++i)f.left_hands.push_back(r.hand());
    n=r.count();f.right_hands.reserve(n);for(std::uint32_t i=0;i<n;++i)f.right_hands.push_back(r.hand());
    n=r.count();f.speech.reserve(n);for(std::uint32_t i=0;i<n;++i){SpeechSample x;x.header=r.header();x.text=r.str();x.phoneme=r.str();x.viseme=r.str();x.normalized_energy=r.f32();x.pitch_hz=r.f32();f.speech.push_back(std::move(x));}
    n=r.count();f.emotions.reserve(n);for(std::uint32_t i=0;i<n;++i){EmotionSample x;x.header=r.header();x.label=r.str();x.intensity=r.f32();f.emotions.push_back(std::move(x));}
    return f;
}
}

std::vector<std::uint8_t> serialize_session(const Session& session){
    const auto check=validate(session);if(!check.ok)throw std::invalid_argument(check.error);
    Writer w;for(auto b:magic)w.u8(b);w.u32(session.schema_version_major);w.u32(session.schema_version_minor);
    w.str(session.session_id);w.str(session.timebase);w.count(session.actors.size());
    for(const auto& a:session.actors){w.str(a.id);w.str(a.display_name);}
    w.count(session.frames.size());for(const auto& f:session.frames)write_frame(w,f);
    return std::move(w.data);
}

bool deserialize_session(std::span<const std::uint8_t> bytes,Session& output,std::string* error){
    try{
        Reader r(bytes);for(auto b:magic)if(r.u8()!=b)throw std::runtime_error("Invalid session archive magic");
        Session candidate;candidate.schema_version_major=r.u32();candidate.schema_version_minor=r.u32();
        candidate.session_id=r.str();candidate.timebase=r.str();
        auto n=r.count();candidate.actors.reserve(n);for(std::uint32_t i=0;i<n;++i)candidate.actors.push_back({r.str(),r.str()});
        n=r.count();candidate.frames.reserve(n);for(std::uint32_t i=0;i<n;++i)candidate.frames.push_back(read_frame(r));
        if(r.pos!=bytes.size())throw std::runtime_error("Trailing bytes after session archive");
        const auto check=validate(candidate);if(!check.ok)throw std::runtime_error(check.error);
        output=std::move(candidate);if(error)error->clear();return true;
    }catch(const std::exception& e){if(error)*error=e.what();return false;}
}
} // namespace bdfr::performance
