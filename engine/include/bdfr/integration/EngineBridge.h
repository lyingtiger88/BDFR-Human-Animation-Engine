#pragma once

#include "bdfr/performance/CanonicalSession.h"
#include "bdfr/core/CurveMixer.h"
#include "bdfrpc/FusionCore.h"
#include "bdfrpc/Types.h"

#include <string>

namespace bdfr::integration {

// Converts a synchronized/fused capture frame into the versioned engine model.
// source_time_ns is copied from the capture frame; session_time_ns must be
// supplied by the caller after clock normalization.
performance::PerformanceFrame fuse_to_engine_frame(\n    const bdfrpc::SyncedFrameSet& source,\n    performance::Nanoseconds session_time_ns);\n\nstd::vector<performance::FaceCurve> mix_face_curves(\n    const std::vector<performance::FaceCurve>& base,\n    std::vector<bdfr::CurveLayer> layers);\n\nperformance::PerformanceFrame to_engine_frame(
    const bdfrpc::FusedPerformanceFrame& source,
    performance::Nanoseconds session_time_ns,
    const std::string& source_id = "bdfrpc-fusion");

} // namespace bdfr::integration
