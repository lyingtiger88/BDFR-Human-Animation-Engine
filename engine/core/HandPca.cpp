#include "bdfr/performance/HandPca.h"

#include <cmath>
#include <string>

namespace bdfr::performance {
namespace {
Quaternionf axis_angle(float x, float y, float z) {
    const float angle = std::sqrt(x*x + y*y + z*z);
    if (!std::isfinite(angle) || angle < 1.0e-8F) return {};
    const float scale = std::sin(angle * 0.5F) / angle;
    return {x * scale, y * scale, z * scale, std::cos(angle * 0.5F)};
}
constexpr const char* digits[] = {"index", "middle", "ring", "pinky", "thumb"};
}

bool HandPcaBasis::valid() const noexcept {
    if (component_count == 0 || component_count > 6) return false;
    for (float value : mean) if (!std::isfinite(value)) return false;
    for (float value : components) if (!std::isfinite(value)) return false;
    return true;
}

bool expand_hand_pca(
    const std::array<float, 6>& coefficients,
    const HandPcaBasis& basis,
    bool left_hand,
    HandSample& output,
    const SampleHeader& header) {
    if (!basis.valid() || header.source_id.empty()) return false;
    for (float value : coefficients) if (!std::isfinite(value)) return false;

    std::array<float, 45> pose = basis.mean;
    for (std::size_t component = 0; component < basis.component_count; ++component) {
        for (std::size_t value = 0; value < pose.size(); ++value) {
            pose[value] += coefficients[component] *
                basis.components[component * pose.size() + value];
        }
    }
    for (float value : pose) if (!std::isfinite(value)) return false;

    HandSample candidate;
    candidate.header = header;
    const std::string side = left_hand ? "left_" : "right_";
    candidate.joints[0] = {"wrist", -1, {}, {}, header.confidence};
    std::size_t joint_index = 1;
    std::size_t pose_index = 0;
    for (const char* digit : digits) {
        const std::size_t metacarpal = joint_index;
        candidate.joints[joint_index++] = {
            side + digit + "_metacarpal", 0, {}, {}, header.confidence
        };
        std::int32_t parent = static_cast<std::int32_t>(metacarpal);
        for (int segment = 1; segment <= 3; ++segment) {
            const float x = pose[pose_index++];
            const float y = pose[pose_index++];
            const float z = pose[pose_index++];
            candidate.joints[joint_index] = {
                side + digit + "_" + std::to_string(segment),
                parent, {}, axis_angle(x, y, z), header.confidence
            };
            parent = static_cast<std::int32_t>(joint_index);
            ++joint_index;
        }
    }
    output = std::move(candidate);
    return true;
}

} // namespace bdfr::performance
