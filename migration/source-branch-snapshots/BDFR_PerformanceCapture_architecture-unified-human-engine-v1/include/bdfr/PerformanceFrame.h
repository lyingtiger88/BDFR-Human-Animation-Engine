#pragma once

#include <cmath>
#include <cstdint>
#include <string>
#include <vector>

namespace bdfr {

enum class PerformanceDomain {
    Face, Head, Body, LeftHand, RightHand, Gaze, Audio, Event
};

enum class SampleValidity {
    Valid, Inferred, Occluded, Missing, Rejected
};

struct TimePoint {
    double session_time_seconds{0.0};
    double source_time_seconds{0.0};
    std::string source_clock_id;
    std::uint64_t sequence{0};
};

struct CoordinateSpace {
    std::string id;
    std::string handedness;
    std::string up_axis;
    std::string forward_axis;
    std::string linear_unit;
    std::string rotation_representation;
    std::string transform_space; // "world" or "local"
};

struct Confidence {
    float value{0.0F};
    SampleValidity validity{SampleValidity::Missing};
};

struct FaceCurve {
    std::string name;
    float value{0.0F};
    Confidence confidence;
};

struct JointSample {
    std::string name;
    // Local rotation in the declared rotation representation.
    float rotation_x{0.0F};
    float rotation_y{0.0F};
    float rotation_z{0.0F};
    float rotation_w{1.0F};
    Confidence confidence;
};

struct PerformanceSample {
    std::uint32_t schema_major{1};
    std::uint32_t schema_minor{0};
    std::string session_id;
    std::string take_id;
    std::string actor_id;
    std::string source_id;
    std::string provider_id;
    PerformanceDomain domain{PerformanceDomain::Body};
    TimePoint time;
    CoordinateSpace coordinate_space;
    Confidence confidence;
    std::vector<FaceCurve> face_curves;
    std::vector<JointSample> joints;
};

inline bool valid_confidence(const Confidence& confidence) noexcept {
    return std::isfinite(confidence.value) &&
           confidence.value >= 0.0F &&
           confidence.value <= 1.0F;
}

inline bool validate(const PerformanceSample& sample,
                     std::string* reason = nullptr) {
    const auto fail = [reason](const char* message) {
        if (reason) *reason = message;
        return false;
    };

    if (sample.schema_major != 1)
        return fail("unsupported schema major version");
    if (sample.session_id.empty() || sample.take_id.empty() ||
        sample.actor_id.empty() || sample.source_id.empty() ||
        sample.provider_id.empty())
        return fail("session, take, actor, source and provider IDs are required");
    if (!std::isfinite(sample.time.session_time_seconds) ||
        sample.time.session_time_seconds < 0.0 ||
        !std::isfinite(sample.time.source_time_seconds))
        return fail("timestamps must be finite and session time non-negative");
    if (sample.time.source_clock_id.empty())
        return fail("source clock ID is required");
    if (!valid_confidence(sample.confidence))
        return fail("sample confidence must be finite and within [0, 1]");

    const bool spatial = sample.domain == PerformanceDomain::Head ||
                         sample.domain == PerformanceDomain::Body ||
                         sample.domain == PerformanceDomain::LeftHand ||
                         sample.domain == PerformanceDomain::RightHand ||
                         sample.domain == PerformanceDomain::Gaze;
    if (spatial) {
        const auto& c = sample.coordinate_space;
        if (c.id.empty() || c.handedness.empty() || c.up_axis.empty() ||
            c.forward_axis.empty() || c.linear_unit.empty() ||
            c.rotation_representation.empty() ||
            (c.transform_space != "world" && c.transform_space != "local"))
            return fail("spatial samples require a complete coordinate-space declaration");
    }

    for (const auto& curve : sample.face_curves) {
        if (curve.name.empty() || !std::isfinite(curve.value) ||
            !valid_confidence(curve.confidence))
            return fail("face curves require a name, finite value and valid confidence");
    }
    for (const auto& joint : sample.joints) {
        if (joint.name.empty() ||
            !std::isfinite(joint.rotation_x) ||
            !std::isfinite(joint.rotation_y) ||
            !std::isfinite(joint.rotation_z) ||
            !std::isfinite(joint.rotation_w) ||
            !valid_confidence(joint.confidence))
            return fail("joint samples require a name, finite rotation and valid confidence");
    }

    if (reason) reason->clear();
    return true;
}

} // namespace bdfr
