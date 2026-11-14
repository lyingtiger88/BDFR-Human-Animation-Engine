#include "bdfr/performance/CoordinateProfile.h"

#include <algorithm>
#include <cmath>

namespace bdfr::performance {
namespace {
using Matrix3 = std::array<std::array<double, 3>, 3>;

Matrix3 quaternion_matrix(const Quaternionf& input) {
    double x=input.x, y=input.y, z=input.z, w=input.w;
    const double norm=std::sqrt(x*x+y*y+z*z+w*w);
    if (norm <= 1.0e-12) return {};
    x/=norm; y/=norm; z/=norm; w/=norm;
    return {{
        {{1-2*(y*y+z*z), 2*(x*y-z*w), 2*(x*z+y*w)}},
        {{2*(x*y+z*w), 1-2*(x*x+z*z), 2*(y*z-x*w)}},
        {{2*(x*z-y*w), 2*(y*z+x*w), 1-2*(x*x+y*y)}}
    }};
}

Quaternionf matrix_quaternion(const Matrix3& m) {
    double x=0, y=0, z=0, w=1;
    const double trace=m[0][0]+m[1][1]+m[2][2];
    if (trace > 0) {
        const double s=std::sqrt(trace+1.0)*2.0;
        w=0.25*s; x=(m[2][1]-m[1][2])/s;
        y=(m[0][2]-m[2][0])/s; z=(m[1][0]-m[0][1])/s;
    } else if (m[0][0] > m[1][1] && m[0][0] > m[2][2]) {
        const double s=std::sqrt(1.0+m[0][0]-m[1][1]-m[2][2])*2.0;
        w=(m[2][1]-m[1][2])/s; x=0.25*s;
        y=(m[0][1]+m[1][0])/s; z=(m[0][2]+m[2][0])/s;
    } else if (m[1][1] > m[2][2]) {
        const double s=std::sqrt(1.0+m[1][1]-m[0][0]-m[2][2])*2.0;
        w=(m[0][2]-m[2][0])/s; x=(m[0][1]+m[1][0])/s;
        y=0.25*s; z=(m[1][2]+m[2][1])/s;
    } else {
        const double s=std::sqrt(1.0+m[2][2]-m[0][0]-m[1][1])*2.0;
        w=(m[1][0]-m[0][1])/s; x=(m[0][2]+m[2][0])/s;
        y=(m[1][2]+m[2][1])/s; z=0.25*s;
    }
    const double norm=std::sqrt(x*x+y*y+z*z+w*w);
    if (norm <= 1.0e-12) return {};
    return {static_cast<float>(x/norm), static_cast<float>(y/norm),
            static_cast<float>(z/norm), static_cast<float>(w/norm)};
}
}

bool CoordinateProfile::valid() const noexcept {
    if (!std::isfinite(unit_scale) || unit_scale <= 0.0F) return false;
    bool seen[3]{false, false, false};
    for (std::size_t i=0; i<3; ++i) {
        if (axis[i] > 2 || seen[axis[i]] || (sign[i] != 1 && sign[i] != -1))
            return false;
        seen[axis[i]]=true;
    }
    return true;
}

bool CoordinateProfile::transform_position(
    const Vec3f& source, Vec3f& output) const noexcept {
    if (!valid() || !std::isfinite(source.x) || !std::isfinite(source.y) ||
        !std::isfinite(source.z)) return false;
    const float values[3]{source.x, source.y, source.z};
    output = {
        values[axis[0]] * static_cast<float>(sign[0]) * unit_scale,
        values[axis[1]] * static_cast<float>(sign[1]) * unit_scale,
        values[axis[2]] * static_cast<float>(sign[2]) * unit_scale
    };
    return true;
}

bool CoordinateProfile::transform_direction(
    const Vec3f& source, Vec3f& output) const noexcept {
    if (!valid() || !std::isfinite(source.x) || !std::isfinite(source.y) ||
        !std::isfinite(source.z)) return false;
    const float values[3]{source.x, source.y, source.z};
    output = {
        values[axis[0]] * static_cast<float>(sign[0]),
        values[axis[1]] * static_cast<float>(sign[1]),
        values[axis[2]] * static_cast<float>(sign[2])
    };
    return true;
}

bool CoordinateProfile::transform_rotation(
    const Quaternionf& source, Quaternionf& output) const noexcept {
    if (!valid() || !std::isfinite(source.x) || !std::isfinite(source.y) ||
        !std::isfinite(source.z) || !std::isfinite(source.w)) return false;
    const Matrix3 rotation=quaternion_matrix(source);
    const double norm=std::sqrt(
        static_cast<double>(source.x)*source.x +
        static_cast<double>(source.y)*source.y +
        static_cast<double>(source.z)*source.z +
        static_cast<double>(source.w)*source.w);
    if (norm <= 1.0e-12) return false;

    Matrix3 basis{};
    for(std::size_t i=0;i<3;++i) basis[i][axis[i]]=static_cast<double>(sign[i]);
    Matrix3 intermediate{}, transformed{};
    for(std::size_t i=0;i<3;++i)
        for(std::size_t j=0;j<3;++j)
            for(std::size_t k=0;k<3;++k)
                intermediate[i][j]+=basis[i][k]*rotation[k][j];
    for(std::size_t i=0;i<3;++i)
        for(std::size_t j=0;j<3;++j)
            for(std::size_t k=0;k<3;++k)
                transformed[i][j]+=intermediate[i][k]*basis[j][k];
    output=matrix_quaternion(transformed);
    return true;
}

} // namespace bdfr::performance
