#pragma once

#include "bdfr/performance/CanonicalSession.h"

#include <array>
#include <cstdint>

namespace bdfr::performance {

// Signed axis permutation from source coordinates to canonical engine space.
// axis[i] selects the source axis used for output axis i; sign[i] is +/-1.
struct CoordinateProfile {
    std::array<std::uint8_t, 3> axis{0, 1, 2};
    std::array<std::int8_t, 3> sign{1, 1, 1};
    float unit_scale{1.0F};

    bool valid() const noexcept;
    bool transform_position(const Vec3f& source, Vec3f& output) const noexcept;
    bool transform_direction(const Vec3f& source, Vec3f& output) const noexcept;
    bool transform_rotation(const Quaternionf& source, Quaternionf& output) const noexcept;
};

} // namespace bdfr::performance
