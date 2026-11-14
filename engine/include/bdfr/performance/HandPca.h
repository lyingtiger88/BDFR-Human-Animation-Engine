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

// Expands six PCA coefficients into the standard 21-landmark hand topology
// (wrist + four joints for each of five digits) stored in the fixed 25-slot
// HandSample container. Slot order is wrist, then index/middle/ring/pinky/thumb;
// each digit has a neutral metacarpal followed by proximal/intermediate/distal
// joints driven by the 15 predicted axis-angle rotations. Unused slots stay empty.
bool expand_hand_pca(
    const std::array<float, 6>& coefficients,
    const HandPcaBasis& basis,
    bool left_hand,
    HandSample& output,
    const SampleHeader& header);

} // namespace bdfr::performance
