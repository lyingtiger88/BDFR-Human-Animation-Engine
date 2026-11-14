#include "bdfr/integration/EngineBridge.h"
#include "bdfr/performance/Validation.h"

#include <cassert>
#include <utility>

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

    bdfr::FacialFrame facial;
    facial.timestampSeconds = 0.25;
    facial.confidence = 0.9F;
    facial.curves["jawOpen"] = 0.4F;
    facial.head = {0.1F, 0.2F, 0.3F};
    facial.gaze = {0.5F, -0.2F, 0.8F};
    auto facial_frame = bdfr::integration::to_engine_frame(facial, 250000000, "facial-camera");
    assert(facial_frame.faces.size() == 1);
    assert(facial_frame.faces[0].header.source_time_ns == 250000000);
    assert(facial_frame.heads[0].channels.size() == 3);
    assert(facial_frame.gaze[0].channels.size() == 2);

    bdfrpc::SkeletonPose pose;
    pose.model = bdfrpc::SkeletonModel::SMPL24;
    pose.timestamp_ns = 300;
    pose.confidence = 0.95F;
    pose.translation = {1.0F, 2.0F, 3.0F};
    pose.joints.push_back({"pelvis", -1, {0.0F, 0.0F, 0.0F}});
    pose.joints.push_back({"spine", 0, {0.0F, 0.0F, 0.5F}});
    pose.shape = {0.1F, 0.2F};
    pose.left_hand_pca = {0.3F};
    auto skeleton = bdfr::integration::to_engine_skeleton(pose, 400, "body-worker");
    assert(skeleton.joints.size() == 2);
    assert(skeleton.joints[1].parent_index == 0);
    assert(skeleton.channels.size() == 3);
    assert(skeleton.root_translation.y == 2.0F);

    bdfrpc::SourceRouter router;
    router.set_route(bdfrpc::Domain::Face, {{"camera-1", 0.5F, 1000000}});
    bdfrpc::SyncedFrameSet synced;
    synced.timestamp_ns = 555;
    bdfrpc::CaptureFrame capture;
    capture.timestamp_ns = 555;
    capture.source_id = "camera-1";
    capture.samples.push_back({bdfrpc::Domain::Face, 0.95F, {0.6F}, {"jawOpen"}});
    synced.frames.push_back(capture);
    auto fused_frame = bdfr::integration::fuse_to_engine_frame(synced, 777, std::move(router));
    assert(fused_frame.session_time_ns == 777);
    assert(fused_frame.faces.size() == 1);

    return 0;
}
