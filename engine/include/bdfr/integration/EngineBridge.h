#pragma once

#include "bdfr/performance/CanonicalSession.h"
#include "bdfr/core/CurveMixer.h"
#include "bdfr/core/FacialTypes.h"
#include "bdfrpc/FusionCore.h"
#include "bdfrpc/Types.h"
#include "bdfrpc/Skeleton.h"
#include "bdfrpc/Retarget.h"
#include "bdfr/audio/WavAudio.h"

#include <string>
#include <vector>

namespace bdfr::integration {

performance::PerformanceFrame to_engine_frame(
    const bdfr::FacialFrame& source,
    performance::Nanoseconds session_time_ns,
    const std::string& source_id = "facial-solver");

performance::RetargetedSkeletonSample retarget_to_engine(
    const performance::SkeletonSample& source,
    const bdfrpc::RetargetProfile& profile);

performance::SkeletonSample to_engine_skeleton(
    const bdfrpc::SkeletonPose& source,
    performance::Nanoseconds session_time_ns,
    const std::string& source_id = "body-solver");

performance::PerformanceFrame fuse_to_engine_frame(
    const bdfrpc::SyncedFrameSet& source,
    performance::Nanoseconds session_time_ns,
    bdfrpc::SourceRouter router);

std::vector<performance::FaceCurve> mix_face_curves(
    const std::vector<performance::FaceCurve>& base,
    std::vector<bdfr::CurveLayer> layers);

performance::PerformanceFrame to_engine_frame(
    const bdfr::audio::AudioBuffer& source,
    performance::Nanoseconds session_time_ns,
    const std::string& source_id = "audio-analysis");

performance::PerformanceFrame to_engine_frame(
    const bdfrpc::FusedPerformanceFrame& source,
    performance::Nanoseconds session_time_ns,
    const std::string& source_id = "bdfrpc-fusion");

} // namespace bdfr::integration
