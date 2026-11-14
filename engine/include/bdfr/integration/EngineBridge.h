#pragma once

#include "bdfr/performance/CanonicalSession.h"
#include "bdfr/core/CurveMixer.h"
#include "bdfrpc/FusionCore.h"
#include "bdfrpc/Types.h"

#include <string>
#include <vector>

namespace bdfr::integration {

performance::PerformanceFrame fuse_to_engine_frame(
    const bdfrpc::SyncedFrameSet& source,
    performance::Nanoseconds session_time_ns);

std::vector<performance::FaceCurve> mix_face_curves(
    const std::vector<performance::FaceCurve>& base,
    std::vector<bdfr::CurveLayer> layers);

performance::PerformanceFrame to_engine_frame(
    const bdfrpc::FusedPerformanceFrame& source,
    performance::Nanoseconds session_time_ns,
    const std::string& source_id = "bdfrpc-fusion");

} // namespace bdfr::integration
