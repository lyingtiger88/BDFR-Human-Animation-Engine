#include "bdfr/performance/Validation.h"

#include <cmath>
#include <unordered_map>

namespace bdfr::performance {
namespace {

bool valid_confidence(float value) {
    return std::isfinite(value) && value >= 0.0F && value <= 1.0F;
}

bool valid_header(const SampleHeader& header) {
    return header.source_time_ns >= 0 &&
           header.session_time_ns >= 0 &&
           valid_confidence(header.confidence) &&
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
            if (!valid_header(face.header))
                return {false, "Invalid face sample header"};
            for (const auto& curve : face.curves) {
                if (curve.name.empty() || !std::isfinite(curve.value))
                    return {false, "Face curves require names and finite values"};
            }
        }
        for (const auto& head : frame.heads) {
            if (!valid_header(head.header) || !valid_channels(head.channels))
                return {false, "Invalid head sample or channels"};
        }
        for (const auto& gaze : frame.gaze) {
            if (!valid_header(gaze.header) || !valid_channels(gaze.channels))
                return {false, "Invalid gaze sample or channels"};
        }
        for (const auto& body : frame.bodies) {
            if (!valid_header(body.header) || !valid_channels(body.channels))
                return {false, "Invalid body sample or channels"};
            for (std::size_t i = 0; i < body.joints.size(); ++i) {
                const auto& joint = body.joints[i];
                if (!valid_joint(joint))
                    return {false, "Invalid skeletal joint"};
                if (joint.parent_index >= 0 &&
                    static_cast<std::size_t>(joint.parent_index) >= body.joints.size())
                    return {false, "Skeletal joint parent index is out of range"};
                if (joint.parent_index == static_cast<std::int32_t>(i))
                    return {false, "Skeletal joint cannot parent itself"};
            }
        }
        for (const auto& hand : frame.left_hands) {
            if (!valid_header(hand.header) || !valid_channels(hand.channels))
                return {false, "Invalid left-hand sample or channels"};
        }
        for (const auto& hand : frame.right_hands) {
            if (!valid_header(hand.header) || !valid_channels(hand.channels))
                return {false, "Invalid right-hand sample or channels"};
        }
        for (const auto& speech : frame.speech) {
            if (!valid_header(speech.header) ||
                !std::isfinite(speech.normalized_energy) ||
                !std::isfinite(speech.pitch_hz))
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
