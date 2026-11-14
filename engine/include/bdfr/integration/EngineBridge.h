#pragma once

#include "bdfr/performance/CanonicalSession.h"
#include "bdfr/performance/HandPca.h"
#include "bdfr/performance/CoordinateProfile.h"
#include "bdfr/core/CurveMixer.h"
#include "bdfr/core/FacialTypes.h"
#include "bdfrpc/FusionCore.h"
#include "bdfrpc/Types.h"
#include "bdfrpc/Skeleton.h"
#include "bdfrpc/Retarget.h"
#include "bdfr/audio/WavAudio.h"
#include "bdfr/audio/AudioBehavior.h"
#include "bdfr/speech/TextSpeech.h"
#include "bdfr/speech/LanguageProfile.h"
#include "bdfr/speech/SpeechRecognitionProvider.h"
#include "bdfr/expression/BehaviorEngine.h"
#include "bdfr/expression/EmotionEngine.h"

#include <string>
#include <vector>

namespace bdfr::integration {

struct TimedPhoneme {
    std::string phoneme;
    double start_seconds = 0.0;
    double duration_seconds = 0.0;
    float confidence = 1.0F;
};

struct AlignedSpeechFrames {
    std::string transcript;
    float confidence{0.0F};
    std::vector<performance::PerformanceFrame> frames;
};

bool process_audio_speech(
    const bdfr::speech::SpeechRecognitionProvider& provider,
    const bdfr::audio::AudioBuffer& audio,
    const bdfr::speech::LanguageProfile& language,
    performance::Nanoseconds session_start_ns,
    AlignedSpeechFrames& output,
    std::string* error = nullptr,
    const std::string& source_id = "asr-provider");

std::vector<performance::FaceCurve> sample_phoneme_lipsync_curves(
    const std::vector<TimedPhoneme>& phonemes,
    const bdfr::speech::LanguageProfile& language,
    double time_seconds,
    double coarticulation_seconds = 0.045);

std::vector<performance::PerformanceFrame> to_engine_phoneme_frames(
    const std::vector<TimedPhoneme>& phonemes,
    const bdfr::speech::LanguageProfile& language,
    performance::Nanoseconds session_start_ns,
    const std::string& source_id = "phoneme-alignment");

performance::PerformanceFrame to_engine_frame(
    const bdfr::FacialFrame& source,
    performance::Nanoseconds session_time_ns,
    const std::string& source_id = "facial-solver");

performance::RetargetedSkeletonSample retarget_to_engine(
    const performance::SkeletonSample& source,
    const bdfrpc::RetargetProfile& profile);

bool transform_skeleton_coordinates(
    performance::SkeletonSample& skeleton,
    const performance::CoordinateProfile& profile);

bool transform_hand_coordinates(
    performance::HandSample& hand,
    const performance::CoordinateProfile& profile);

performance::HandSample to_engine_hand(
    const std::vector<float>& pca_coefficients,
    const performance::HandPcaBasis& basis,
    bool left_hand,
    performance::Nanoseconds source_time_ns,
    performance::Nanoseconds session_time_ns,
    const std::string& source_id,
    float confidence = 1.0F);

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

performance::PerformanceFrame sample_emotion(
    bdfr::expression::Emotion emotion,
    float intensity,
    double time_seconds,
    performance::Nanoseconds session_start_ns,
    const std::string& source_id = "emotion-engine");

performance::PerformanceFrame sample_instant_event(
    const bdfr::expression::InstantEvent& event,
    double time_seconds,
    performance::Nanoseconds session_start_ns,
    const std::string& source_id = "instant-event");

performance::PerformanceFrame sample_procedural_face(
    const bdfr::expression::BehaviorProfile& profile,
    double time_seconds,
    performance::Nanoseconds session_start_ns,
    std::uint32_t personality_seed = 0,
    const std::string& source_id = "procedural-behavior");

std::vector<performance::FaceCurve> sample_text_lipsync_curves(
    const std::string& text,
    double time_seconds,
    const bdfr::speech::TextSpeechOptions& options = {});

std::vector<performance::SpeechSample> plan_text_speech(
    const std::string& text,
    performance::Nanoseconds session_start_ns,
    const std::string& source_id = "text-speech",
    const bdfr::speech::TextSpeechOptions& options = {});

std::vector<performance::PerformanceFrame> plan_audio_behavior(
    const bdfr::audio::AudioBuffer& source,
    performance::Nanoseconds session_start_ns,
    const std::string& source_id = "audio-behavior");

performance::PerformanceFrame to_engine_frame(
    const bdfr::audio::AudioBuffer& source,
    performance::Nanoseconds session_time_ns,
    const std::string& source_id = "audio-analysis");

performance::PerformanceFrame to_engine_frame(
    const bdfrpc::FusedPerformanceFrame& source,
    performance::Nanoseconds session_time_ns,
    const std::string& source_id = "bdfrpc-fusion");

} // namespace bdfr::integration
