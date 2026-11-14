#include "bdfr/performance/SessionCodec.h"
#include "bdfr/performance/Validation.h"

#include <cassert>
#include <cstdint>
#include <string>
#include <limits>
#include <vector>

using namespace bdfr::performance;

static SampleHeader header(const char* source) {
    SampleHeader h;
    h.source_time_ns = 10;
    h.session_time_ns = 20;
    h.confidence = 0.9F;
    h.source_id = source;
    return h;
}

int main() {
    Session original;
    original.session_id = "roundtrip-session";
    original.actors.push_back({"actor-1", "Performer"});
    PerformanceFrame frame;
    frame.session_time_ns = 20;

    FaceSample face;
    face.header = header("face-camera");
    face.curves.push_back({"jawOpen", 0.42F});
    face.head_translation = {1.0F, 2.0F, 3.0F};
    frame.faces.push_back(face);

    HeadSample head;
    head.header = header("head-tracker");
    head.channels.push_back({"yaw", 0.25F, 0.8F});
    frame.heads.push_back(head);

    GazeSample gaze;
    gaze.header = header("eye-tracker");
    gaze.channels.push_back({"left_x", 0.1F, 0.95F});
    frame.gaze.push_back(gaze);

    SkeletonSample body;
    body.header = header("body-solver");
    body.skeleton_profile = "SMPL24";
    body.joints.push_back({"pelvis", -1, {}, {}, 1.0F});
    body.joints.push_back({"spine1", 0, {}, {}, 0.95F});
    body.channels.push_back({"shape_0", 0.12F, 0.9F});
    frame.bodies.push_back(body);

    RetargetedSkeletonSample retargeted;
    retargeted.header = header("retargeter");
    retargeted.profile_name = "Unreal Mannequin";
    retargeted.bones.push_back({"spine_01", {}});
    frame.retargeted_bodies.push_back(retargeted);

    HandSample left;
    left.header = header("left-hand");
    left.channels.push_back({"index_tip_flex", 0.3F, 0.8F});
    left.has_palm_pose = true;
    left.palm_position = {0.1F, 0.2F, 0.3F};
    frame.left_hands.push_back(left);

    HandSample right;
    right.header = header("right-hand");
    right.channels.push_back({"thumb_flex", 0.7F, 0.9F});
    frame.right_hands.push_back(right);

    SpeechSample speech;
    speech.header = header("audio-worker");
    speech.text = "hello";
    speech.phoneme = "HH";
    speech.viseme = "viseme_sil";
    speech.normalized_energy = 0.5F;
    speech.pitch_hz = 120.0F;
    frame.speech.push_back(speech);

    EmotionSample emotion;
    emotion.header = header("emotion-worker");
    emotion.label = "happy";
    emotion.intensity = 0.75F;
    frame.emotions.push_back(emotion);

    original.frames.push_back(frame);
    const auto bytes = serialize_session(original);
    assert(bytes.size() > 8);

    Session decoded;
    std::string error;
    assert(deserialize_session(bytes, decoded, &error));
    assert(error.empty());
    assert(decoded.session_id == original.session_id);
    assert(decoded.actors.size() == 1);
    assert(decoded.frames.size() == 1);
    const auto& out = decoded.frames.front();
    assert(out.faces.front().curves.front().name == "jawOpen");
    assert(out.heads.front().channels.front().name == "yaw");
    assert(out.gaze.front().channels.front().name == "left_x");
    assert(out.bodies.front().joints[1].parent_index == 0);
    assert(out.retargeted_bodies.front().bones.front().target_bone == "spine_01");
    assert(out.left_hands.front().has_palm_pose);
    assert(out.right_hands.front().channels.front().value == 0.7F);
    assert(out.speech.front().phoneme == "HH");
    assert(out.emotions.front().label == "happy");

    const auto second = serialize_session(decoded);
    assert(second == bytes); // deterministic round trip

    auto truncated = bytes;
    truncated.pop_back();
    Session unchanged;
    unchanged.session_id = "must-not-be-overwritten";
    assert(!deserialize_session(truncated, unchanged, &error));
    assert(!error.empty());
    assert(unchanged.session_id == "must-not-be-overwritten");

    auto bad_magic = bytes;
    bad_magic[0] = 0;
    assert(!deserialize_session(bad_magic, unchanged, &error));

    Session future = original;
    future.schema_version_minor = schema_minor + 1;
    assert(!validate(future).ok);

    Session non_finite = original;
    non_finite.frames[0].faces[0].head_translation.x = std::numeric_limits<float>::quiet_NaN();
    assert(!validate(non_finite).ok);

    Session invalid = original;
    invalid.session_id.clear();
    bool threw = false;
    try { (void)serialize_session(invalid); }
    catch (const std::invalid_argument&) { threw = true; }
    assert(threw);

    return 0;
}
