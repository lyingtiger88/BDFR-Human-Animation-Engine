#pragma once

#include "bdfr/performance/CanonicalSession.h"

#include <array>
#include <cstddef>

namespace bdfr::performance {

// Model-specific PCA basis supplied by an authorized MANO/SMPL-X asset pack.
// Matrix layout is row-major: 6 components x 45 axis-angle values.
struct HandPcaBasis {
    std::array<float, 45> mean{};
    std::array<float, 270> components{};
    std::size_t component_count{6};
    bool valid() const noexcept;
};

// Expands six PCA coefficients into a canonical 25-joint hand:
// wrist + four joints for each of five digits. The first digit joint is
// a neutral metacarpal; the 15 predicted rotations drive proximal,
// intermediate and distal joints.
bool expand_hand_pca(
    const std::array<float, 6>& coefficients,
    const HandPcaBasis& basis,
    bool left_hand,
    HandSample& output,
    const SampleHeader& header);

} // namespace bdfr::performance
