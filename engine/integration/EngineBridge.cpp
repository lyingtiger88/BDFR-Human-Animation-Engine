#include "bdfr/integration/EngineBridge.h"

#include <iomanip>
#include <sstream>
#include <utility>

namespace bdfr::integration {
namespace {

performance::SampleHeader make_header(
    const bdfrpc::FusedPerformanceFrame& source,
    performance::Nanoseconds session_time_ns,
    const std::string& source_id,
    float confidence) {
    performance::SampleHeader header;
    header.source_time_ns = source.timestamp_ns;
    header.session_time_ns = session_time_ns;
    header.confidence = confidence;
    header.valid = true;
    header.space = performance::CoordinateSpace::Unknown;
    header.source_id = source_id;
    return header;
}

std::string channel_name(const bdfrpc::DomainSample& sample, std::size_t index) {
    if (sample.channels.size() == sample.values.size() && !sample.channels.empty())
        return sample.channels[index];
    std::ostringstream name;
    name << "channel_" << std::setw(3) << std::setfill('0') << index;
    return name.str();
}

std::vector<performance::ScalarChannel> copy_channels(
    const bdfrpc::DomainSample& sample) {
    std::vector<performance::ScalarChannel> result;
    result.reserve(sample.values.size());
    for (std::size_t i = 0; i < sample.values.size(); ++i)
        result.push_back({channel_name(sample, i), sample.values[i], sample.confidence});
    return result;
}

} // namespace

performance::PerformanceFrame fuse_to_engine_frame(
    const bdfrpc::SyncedFrameSet& source,
    performance::Nanoseconds session_time_ns) {
    bdfrpc::FusionCore fusion;
    return to_engine_frame(fusion.fuse(source), session_time_ns, "bdfrpc-fusion");
}

std::vector<performance::FaceCurve> mix_face_curves(
    const std::vector<performance::FaceCurve>& base,
    std::vector<bdfr::CurveLayer> layers) {
    bdfr::CurveMap base_map;
    for (const auto& curve : base) base_map[curve.name] = curve.value;
    const auto mixed = bdfr::CurveMixer::mix(base_map, std::move(layers));
    std::vector<performance::FaceCurve> output;
    output.reserve(mixed.size());
    for (const auto& item : mixed) output.push_back({item.first, item.second});
    return output;
}

performance::PerformanceFrame to_engine_frame(
    const bdfrpc::FusedPerformanceFrame& source,
    performance::Nanoseconds session_time_ns,
    const std::string& source_id) {
    performance::PerformanceFrame output;
    output.session_time_ns = session_time_ns;

    for (const auto& sample : source.samples) {
        if (!sample.channels_valid()) continue;

        switch (sample.domain) {
        case bdfrpc::Domain::Face: {
            performance::FaceSample face;
            face.header = make_header(source, session_time_ns, source_id, sample.confidence);
            face.curves.reserve(sample.values.size());
            for (std::size_t i = 0; i < sample.values.size(); ++i)
                face.curves.push_back({channel_name(sample, i), sample.values[i]});
            output.faces.push_back(std::move(face));
            break;
        }
        case bdfrpc::Domain::Head: {
            performance::HeadSample head;
            head.header = make_header(source, session_time_ns, source_id, sample.confidence);
            head.channels = copy_channels(sample);
            output.heads.push_back(std::move(head));
            break;
        }
        case bdfrpc::Domain::Body: {
            performance::SkeletonSample body;
            body.header = make_header(source, session_time_ns, source_id, sample.confidence);
            body.skeleton_profile = "source-defined";
            body.channels = copy_channels(sample);
            output.bodies.push_back(std::move(body));
            break;
        }
        case bdfrpc::Domain::LeftHand: {
            performance::HandSample hand;
            hand.header = make_header(source, session_time_ns, source_id, sample.confidence);
            hand.channels = copy_channels(sample);
            output.left_hands.push_back(std::move(hand));
            break;
        }
        case bdfrpc::Domain::RightHand: {
            performance::HandSample hand;
            hand.header = make_header(source, session_time_ns, source_id, sample.confidence);
            hand.channels = copy_channels(sample);
            output.right_hands.push_back(std::move(hand));
            break;
        }
        }
    }
    return output;
}

} // namespace bdfr::integration
