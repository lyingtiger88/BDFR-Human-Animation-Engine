#include "bdfr/core/CameraCalibration.h"

#include <cassert>
#include <cmath>
#include <string>
#include <vector>

int main() {
    using namespace bdfr::core;

    CameraCalibration camera;
    camera.camera_id = "front";
    camera.intrinsics.width = 1920;
    camera.intrinsics.height = 1080;
    camera.intrinsics.fx = 1000.0;
    camera.intrinsics.fy = 1000.0;
    camera.intrinsics.cx = 960.0;
    camera.intrinsics.cy = 540.0;
    camera.rms_reprojection_error = 0.25;
    assert(camera.valid());

    const auto projected = project_point(camera, Point3d{0.0, 0.0, 2.0});
    assert(projected.has_value());
    assert(std::abs(projected->x - 960.0) < 1e-9);
    assert(std::abs(projected->y - 540.0) < 1e-9);

    const auto behind = project_point(camera, Point3d{0.0, 0.0, -1.0});
    assert(!behind.has_value());

    const std::vector<Point3d> world{
        {0.0, 0.0, 2.0}, {0.2, 0.0, 2.0}, {0.0, 0.2, 2.0}
    };
    const std::vector<Point2d> image{
        {960.0, 540.0}, {1060.0, 540.0}, {960.0, 640.0}
    };
    assert(reprojection_rmse(camera, world, image) < 1e-9);

    CalibrationProfile profile;
    assert(profile.upsert(camera));
    assert(profile.find("front") != nullptr);
    assert(!profile.upsert(CameraCalibration{}));

    const std::string encoded = profile.serialize();
    std::string error;
    const auto decoded = CalibrationProfile::deserialize(encoded, &error);
    assert(decoded.has_value());
    assert(error.empty());
    assert(decoded->cameras().size() == 1);
    assert(decoded->find("front") != nullptr);

    const auto malformed = CalibrationProfile::deserialize("bad-header", &error);
    assert(!malformed.has_value());
    assert(!error.empty());

    return 0;
}
