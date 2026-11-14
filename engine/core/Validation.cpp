#include "bdfr/performance/Validation.h"

#include <cmath>
#include <unordered_map>
#include <vector>

namespace bdfr::performance {
namespace {

bool valid_confidence(float value) {
    return std::isfinite(value) && value >= 0.0F && value <= 1.0F;
}

bool valid_header(const SampleHeader& header) {
    return header.source_time_ns >= 0 &&
           header.session_time_ns >= 0 &&
           valid_confidence(header.confidence) &&
           static_cast<std::uint8_t>(header.space) <=
               static_cast<std::uint8_t>(CoordinateSpace::LocalJoint) &&
           !header.source_id.empty();
}

bool valid_channels(const std::vector<ScalarChannel>& channels) {
    for (const auto& channel : channels) {
        if (channel.name.empty() || !std::isfinite(channel.value) ||
            !valid_confidence(channel.confidence))
            return false;
    }
    return true;
}

bool valid_vec(const Vec3f& value) {
    return std::isfinite(value.x) && std::isfinite(value.y) && std::isfinite(value.z);
}

bool valid_quat(const Quaternionf& value) {
    return std::isfinite(value.x) && std::isfinite(value.y) &&
           std::isfinite(value.z) && std::isfinite(value.w);
}

bool valid_joint(const JointSample& joint) {
    return !joint.name.empty() &&
           std::isfinite(joint.translation.x) &&
           std::isfinite(joint.translation.y) &&
           std::isfinite(joint.translation.z) &&
           std::isfinite(joint.rotation.x) &&
           std::isfinite(joint.rotation.y) &&
           std::isfinite(joint.rotation.z) &&
           std::isfinite(joint.rotation.w) &&
           valid_confidence(joint.confidence);
}

} // namespace

ValidationResult validate(const Session& session) {
    if (session.schema_version_major != schema_major)
        return {false, "Unsupported canonical session major version"};
    if (session.schema_version_minor > schema_minor)
        return {false, "Session uses a newer unsupported minor schema version"};
    if (session.session_id.empty())
        return {false, "Session ID must not be empty"};
    if (session.timebase != "nanoseconds")
        return {false, "Unsupported session timebase"};

    std::unordered_map<std::string, bool> ids;
    for (const auto& actor : session.actors) {
        if (actor.id.empty())
            return {false, "Actor ID must not be empty"};
        if (!ids.emplace(actor.id, true).second)
            return {false, "Duplicate actor ID"};
    }

    Nanoseconds previous = -1;
    for (const auto& frame : session.frames) {
        if (frame.session_time_ns < 0)
            return {false, "Negative session timestamp"};
        if (frame.session_time_ns < previous)
            return {false, "Frames are not ordered by session time"};
        previous = frame.session_time_ns;

        for (const auto& face : frame.faces) {
            if (!valid_header(face.header) || !valid_vec(face.head_translation) ||
                !valid_quat(face.head_rotation) || !valid_vec(face.gaze_direction))
                return {false, "Invalid face sample header or transforms"};
            for (const auto& curve : face.curves) {
                if (curve.name.empty() || !std::isfinite(curve.value))
                    return {false, "Face curves require names and finite values"};
            }
        }
        for (const auto& head : frame.heads) {
            if (!valid_header(head.header) || !valid_vec(head.translation) ||
                !valid_quat(head.rotation) || !valid_channels(head.channels))
                return {false, "Invalid head sample or channels"};
        }
        for (const auto& gaze : frame.gaze) {
            if (!valid_header(gaze.header) || !valid_channels(gaze.channels))
                return {false, "Invalid gaze sample or channels"};
        }
        for (const auto& body : frame.bodies) {
            if (!valid_header(body.header) || !valid_vec(body.root_translation) ||
                !valid_quat(body.root_rotation) || !valid_channels(body.channels))
                return {false, "Invalid body sample or channels"};
            for (std::size_t i = 0; i < body.joints.size(); ++i) {
                const auto& joint = body.joints[i];
                if (!valid_joint(joint))
                    return {false, "Invalid skeletal joint"};
                if (joint.parent_index < -1)
                    return {false, "Skeletal joint parent index is invalid"};
                if (joint.parent_index >= 0 &&
                    static_cast<std::size_t>(joint.parent_index) >= body.joints.size())
                    return {false, "Skeletal joint parent index is out of range"};
                if (joint.parent_index == static_cast<std::int32_t>(i))
                    return {false, "Skeletal joint cannot parent itself"};
            }

            // Reject longer parent cycles without recursive traversal (safe for deep rigs).
            std::vector<std::uint8_t> visit(body.joints.size(), 0);
            for (std::size_t start = 0; start < body.joints.size(); ++start) {
                if (visit[start] != 0) continue;
                std::vector<std::size_t> chain;
                std::size_t current = start;
                while (visit[current] == 0) {
                    visit[current] = 1;
                    chain.push_back(current);
                    const auto parent = body.joints[current].parent_index;
                    if (parent < 0) break;
                    current = static_cast<std::size_t>(parent);
                }
                if (visit[current] == 1 && body.joints[current].parent_index >= 0)
                    return {false, "Skeletal joint hierarchy contains a cycle"};
                for (const auto index : chain) visit[index] = 2;
            }
        }
        for (const auto& retargeted : frame.retargeted_bodies) {
            if (!valid_header(retargeted.header) || retargeted.profile_name.empty() ||
                !valid_vec(retargeted.root_translation) || !valid_quat(retargeted.root_rotation))
                return {false, "Invalid retargeted skeleton sample"};
            for (const auto& bone : retargeted.bones) {
                if (bone.target_bone.empty() ||
                    !std::isfinite(bone.rotation.x) ||
                    !std::isfinite(bone.rotation.y) ||
                    !std::isfinite(bone.rotation.z) ||
                    !std::isfinite(bone.rotation.w))
                    return {false, "Invalid retargeted bone"};
            }
        }
        for (const auto& hand : frame.left_hands) {
            if (!valid_header(hand.header) || !valid_channels(hand.channels) ||
                !valid_vec(hand.palm_position) || !valid_quat(hand.palm_rotation))
                return {false, "Invalid left-hand sample or channels"};
        }
        for (const auto& hand : frame.right_hands) {
            if (!valid_header(hand.header) || !valid_channels(hand.channels) ||
                !valid_vec(hand.palm_position) || !valid_quat(hand.palm_rotation))
                return {false, "Invalid right-hand sample or channels"};
        }
        for (const auto& speech : frame.speech) {
            if (!valid_header(speech.header) ||
                !valid_confidence(speech.normalized_energy) ||
                !std::isfinite(speech.pitch_hz) || speech.pitch_hz < 0.0F)
                return {false, "Invalid speech sample"};
        }
        for (const auto& emotion : frame.emotions) {
            if (!valid_header(emotion.header) || emotion.label.empty() ||
                !valid_confidence(emotion.intensity))
                return {false, "Invalid emotion sample"};
        }
    }
    return {};
}

} // namespace bdfr::performance
