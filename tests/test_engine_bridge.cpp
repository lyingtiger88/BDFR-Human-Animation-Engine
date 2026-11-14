#include "bdfr/integration/EngineBridge.h"
#include "bdfr/performance/Validation.h"

#include <cassert>

int main() {
    bdfrpc::FusedPerformanceFrame fused;
    fused.timestamp_ns = 123456789;
    fused.samples.push_back({bdfrpc::Domain::Face, 0.9F, {0.25F, 0.75F},
                             {"jawOpen", "mouthSmileLeft"}});
    fused.samples.push_back({bdfrpc::Domain::Head, 0.8F, {1.0F, 2.0F},
                             {"head.yaw", "head.pitch"}});
    fused.samples.push_back({bdfrpc::Domain::Body, 0.95F, {0.1F, 0.2F},
                             {"pelvis.tx", "pelvis.ty"}});
    fused.samples.push_back({bdfrpc::Domain::LeftHand, 0.7F, {0.3F},
                             {"indexTip.flex"}});
    fused.samples.push_back({bdfrpc::Domain::RightHand, 0.6F, {0.4F},
                             {"thumbTip.flex"}});

    auto frame = bdfr::integration::to_engine_frame(fused, 987654321, "test-camera-fusion");
    assert(frame.session_time_ns == 987654321);
    assert(frame.faces.size() == 1);
    assert(frame.faces[0].header.source_time_ns == 123456789);
    assert(frame.faces[0].header.session_time_ns == 987654321);
    assert(frame.faces[0].curves.size() == 2);
    assert(frame.faces[0].curves[0].name == "jawOpen");
    assert(frame.heads.size() == 1 && frame.heads[0].channels[0].name == "head.yaw");
    assert(frame.bodies.size() == 1 && frame.bodies[0].channels[1].name == "pelvis.ty");
    assert(frame.left_hands.size() == 1);
    assert(frame.right_hands.size() == 1);

    fused.samples[0].channels = {"only-one-name"};
    auto safe = bdfr::integration::to_engine_frame(fused, 987654321);
    assert(safe.faces.empty());

    bdfr::CurveLayer smile;
    smile.curves["mouthSmileLeft"] = 0.8F;
    smile.weight = 0.5F;
    auto mixed = bdfr::integration::mix_face_curves({{"jawOpen", 0.2F}}, {smile});
    assert(mixed.size() == 2);

    bdfrpc::SourceRouter router;\n    router.set_route(bdfrpc::Domain::Face, {{"camera-1", 0.5F, 1000000}});\n    bdfrpc::SyncedFrameSet synced;
    synced.timestamp_ns = 555;
    bdfrpc::CaptureFrame capture;
    capture.timestamp_ns = 555;\n    capture.source_id = "camera-1";
    capture.samples.push_back({bdfrpc::Domain::Face, 0.95F, {0.6F}, {"jawOpen"}});
    synced.frames.push_back(capture);
    auto fused_frame = bdfr::integration::fuse_to_engine_frame(synced, 777, std::move(router));
    assert(fused_frame.session_time_ns == 777);
    assert(fused_frame.faces.size() == 1);

    return 0;
}
