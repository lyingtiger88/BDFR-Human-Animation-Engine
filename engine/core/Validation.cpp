#include "bdfr/performance/Validation.h"
#include <cmath>
#include <unordered_map>
namespace bdfr::performance {
ValidationResult validate(const Session& session) {
    if (session.schema_version_major != schema_major)
        return {false, "Unsupported canonical session major version"};
    if (session.session_id.empty()) return {false, "Session ID must not be empty"};
    if (session.timebase != "nanoseconds") return {false, "Unsupported session timebase"};
    std::unordered_map<std::string, bool> ids;
    for (const auto& actor : session.actors) {
        if (actor.id.empty()) return {false, "Actor ID must not be empty"};
        if (!ids.emplace(actor.id, true).second) return {false, "Duplicate actor ID"};
    }
    Nanoseconds previous = -1;
    for (const auto& frame : session.frames) {
        if (frame.session_time_ns < 0) return {false, "Negative session timestamp"};
        if (frame.session_time_ns < previous) return {false, "Frames are not ordered by session time"};
        previous = frame.session_time_ns;
        for (const auto& face : frame.faces) {
            if (!std::isfinite(face.header.confidence) || face.header.confidence < 0.0F || face.header.confidence > 1.0F)
                return {false, "Face confidence must be finite and in [0,1]"};
            for (const auto& curve : face.curves)
                if (!std::isfinite(curve.value)) return {false, "Face curve value must be finite"};
        }
        for (const auto& body : frame.bodies)
            for (const auto& joint : body.joints)
                if (!std::isfinite(joint.confidence) || joint.confidence < 0.0F || joint.confidence > 1.0F)
                    return {false, "Joint confidence must be finite and in [0,1]"};
    }
    return {};
}
}
