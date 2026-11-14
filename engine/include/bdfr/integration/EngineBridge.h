#pragma once

#include "bdfr/performance/CanonicalSession.h"
#include "bdfrpc/Types.h"

#include <string>

namespace bdfr::integration {

// Converts a synchronized/fused capture frame into the versioned engine model.
// source_time_ns is copied from the capture frame; session_time_ns must be
// supplied by the caller after clock normalization.
performance::PerformanceFrame to_engine_frame(
    const bdfrpc::FusedPerformanceFrame& source,
    performance::Nanoseconds session_time_ns,
    const std::string& source_id = "bdfrpc-fusion");

} // namespace bdfr::integration
