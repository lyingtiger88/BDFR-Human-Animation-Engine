#include "bdfr/performance/HandPca.h"

#include <array>
#include <cassert>
#include <cmath>

using namespace bdfr::performance;

int main() {
    HandPcaBasis basis;
    basis.component_count = 6;
    // Deterministic toy basis: each PCA component controls a distinct
    // axis-angle value. Production coefficients require the licensed model basis.
    basis.components[0] = 1.0F;
    basis.components[45 + 1] = 1.0F;
    std::array<float, 6> coefficients{};
    coefficients[0] = 0.25F;
    coefficients[1] = 0.5F;

    SampleHeader header;
    header.source_id = "hand-pca-test";
    header.confidence = 0.9F;

    HandSample left;
    assert(expand_hand_pca(coefficients, basis, true, left, header));
    assert(left.joints.size() == 25);
    assert(left.joints[0].name == "wrist");
    assert(left.joints[1].name == "left_index_metacarpal");
    assert(left.joints[2].name == "left_index_1");
    assert(left.joints[2].rotation.w < 1.0F);
    assert(left.joints[24].name == "left_thumb_3");

    HandSample right;
    assert(expand_hand_pca(coefficients, basis, false, right, header));
    assert(right.joints[1].name == "right_index_metacarpal");

    coefficients[2] = std::numeric_limits<float>::quiet_NaN();
    assert(!expand_hand_pca(coefficients, basis, true, left, header));
    basis.component_count = 7;
    assert(!expand_hand_pca({}, basis, true, left, header));
    return 0;
}
