#include "bdfr/performance/CoordinateProfile.h"

#include <cassert>
#include <cmath>

using namespace bdfr::performance;

int main() {
    CoordinateProfile profile;
    profile.axis = {1, 0, 2};
    profile.sign = {1, -1, 1};
    profile.unit_scale = 0.01F;
    assert(profile.valid());

    Vec3f position;
    assert(profile.transform_position({100.0F, 200.0F, 300.0F}, position));
    assert(std::abs(position.x - 2.0F) < 1.0e-5F);
    assert(std::abs(position.y + 1.0F) < 1.0e-5F);
    assert(std::abs(position.z - 3.0F) < 1.0e-5F);

    Vec3f direction;
    assert(profile.transform_direction({1.0F, 2.0F, 3.0F}, direction));
    assert(direction.x == 2.0F && direction.y == -1.0F && direction.z == 3.0F);

    const float half = 0.70710678118F;
    Quaternionf rotation{half, 0.0F, 0.0F, half};
    Quaternionf converted;
    assert(profile.transform_rotation(rotation, converted));
    assert(std::abs(converted.x) < 1.0e-4F);
    assert(converted.y < -0.70F);
    assert(std::abs(converted.z) < 1.0e-4F);

    CoordinateProfile invalid = profile;
    invalid.axis = {0, 0, 2};
    assert(!invalid.valid());
    assert(!invalid.transform_position({}, position));

    Quaternionf zero{0.0F, 0.0F, 0.0F, 0.0F};
    assert(!profile.transform_rotation(zero, converted));
    return 0;
}
