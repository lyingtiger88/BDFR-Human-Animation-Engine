#include "bdfr/PerformanceFrame.h"

#include <cassert>
#include <string>

int main() {
    bdfr::PerformanceSample sample;
    sample.session_id = "session-1";
    sample.take_id = "take-1";
    sample.actor_id = "actor-1";
    sample.source_id = "camera-0";
    sample.provider_id = "body-worker";
    sample.domain = bdfr::PerformanceDomain::Body;
    sample.time = {1.25, 12.5, "camera-clock-0", 42};
    sample.coordinate_space = {
        "bdfr.world.v1", "right-handed", "+Z", "+X",
        "meters", "quaternion_xyzw", "local"
    };
    sample.confidence = {0.95F, bdfr::SampleValidity::Valid};
    sample.joints.push_back({
        "pelvis", 0.0F, 0.0F, 0.0F, 1.0F,
        {0.98F, bdfr::SampleValidity::Valid}
    });

    std::string reason;
    assert(bdfr::validate(sample, &reason));
    assert(reason.empty());

    sample.confidence.value = 1.5F;
    assert(!bdfr::validate(sample, &reason));
    assert(!reason.empty());

    sample.confidence.value = 0.95F;
    sample.coordinate_space.forward_axis.clear();
    assert(!bdfr::validate(sample, &reason));

    sample.coordinate_space.forward_axis = "+X";
    sample.time.session_time_seconds = -1.0;
    assert(!bdfr::validate(sample, &reason));

    return 0;
}
