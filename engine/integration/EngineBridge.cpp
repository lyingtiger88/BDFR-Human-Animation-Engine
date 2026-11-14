#include "bdfr/integration/EngineBridge.h"
#include "bdfr/audio/AudioFeatures.h"
#include "bdfr/audio/PitchTracker.h"
#include "bdfr/audio/Prosody.h"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <limits>
#include <sstream>
#include <utility>

namespace bdfr::integration {
namespace {

performance::SampleHeader make_header(
    const bdfrpc::FusedPerformanceFrame& source,
    performance::Nanoseconds session_time_ns,
    const std::string& source_id,
    float confidence) {
    performance::SampleHeader header;
    header.source_time_ns = source.timestamp_ns;
    header.session_time_ns = session_time_ns;
    header.confidence = confidence;
    header.valid = true;
    header.space = performance::CoordinateSpace::Unknown;
    header.source_id = source_id;
    return header;
}

std::string channel_name(const bdfrpc::DomainSample& sample, std::size_t index) {
    if (sample.channels.size() == sample.values.size() && !sample.channels.empty())
        return sample.channels[index];
    std::ostringstream name;
    name << "channel_" << std::setw(3) << std::setfill('0') << index;
    return name.str();
}

std::vector<performance::ScalarChannel> copy_channels(
    const bdfrpc::DomainSample& sample) {
    std::vector<performance::ScalarChannel> result;
    result.reserve(sample.values.size());
    for (std::size_t i = 0; i < sample.values.size(); ++i)
        result.push_back({channel_name(sample, i), sample.values[i], sample.confidence});
    return result;
}

performance::Quaternionf euler_xyz_to_quaternion(float pitch, float yaw, float roll) {
    const float hp = pitch * 0.5F;
    const float hy = yaw * 0.5F;
    const float hr = roll * 0.5F;
    const float cp = std::cos(hp), sp = std::sin(hp);
    const float cy = std::cos(hy), sy = std::sin(hy);
    const float cr = std::cos(hr), sr = std::sin(hr);
    return {
        cr * cp * cy + sr * sp * sy,
        sr * cp * cy - cr * sp * sy,
        cr * sp * cy + sr * cp * sy,
        cr * cp * sy - sr * sp * cy
    };
}

performance::Quaternionf to_engine_quaternion(const bdfrpc::AxisAngle& value) {
    const float angle = std::sqrt(value.x * value.x + value.y * value.y + value.z * value.z);
    if (!std::isfinite(angle) || angle < 1.0e-8F) return {};
    const float half = angle * 0.5F;
    const float scale = std::sin(half) / angle;
    return {value.x * scale, value.y * scale, value.z * scale, std::cos(half)};
}

performance::SampleHeader make_header(
    performance::Nanoseconds source_time_ns,
    performance::Nanoseconds session_time_ns,
    const std::string& source_id,
    float confidence) {
    performance::SampleHeader header;
    header.source_time_ns = source_time_ns;
    header.session_time_ns = session_time_ns;
    header.confidence = confidence;
    header.valid = true;
    header.space = performance::CoordinateSpace::Unknown;
    header.source_id = source_id;
    return header;
}

} // namespace

performance::PerformanceFrame to_engine_frame(
    const bdfr::FacialFrame& source,
    performance::Nanoseconds session_time_ns,
    const std::string& source_id) {
    performance::PerformanceFrame output;
    output.session_time_ns = session_time_ns;
    if (!std::isfinite(source.timestampSeconds) || source.timestampSeconds < 0.0)
        return output;
    const long double source_ns = static_cast<long double>(source.timestampSeconds) * 1000000000.0L;
    if (source_ns > static_cast<long double>(std::numeric_limits<performance::Nanoseconds>::max()))
        return output;

    const auto timestamp_ns = static_cast<performance::Nanoseconds>(std::llround(source_ns));
    performance::FaceSample face;
    face.header = make_header(timestamp_ns, session_time_ns, source_id, source.confidence);
    // Facial HeadPose angles are radians in pitch/yaw/roll order.
    const auto head_rotation = euler_xyz_to_quaternion(
        source.head.pitch, source.head.yaw, source.head.roll);
    face.head_rotation = head_rotation;
    face.gaze_direction = {source.gaze.x, source.gaze.y, 1.0F};
    std::vector<std::pair<std::string, float>> ordered_curves(source.curves.begin(), source.curves.end());
    std::sort(ordered_curves.begin(), ordered_curves.end(),
              [](const auto& a, const auto& b) { return a.first < b.first; });
    for (const auto& item : ordered_curves)
        face.curves.push_back({item.first, item.second});
    output.faces.push_back(std::move(face));

    performance::HeadSample head;
    head.header = make_header(timestamp_ns, session_time_ns, source_id, source.confidence);
    head.rotation = head_rotation;
    head.channels = {
        {"pitch", source.head.pitch, source.confidence},
        {"yaw", source.head.yaw, source.confidence},
        {"roll", source.head.roll, source.confidence}
    };
    output.heads.push_back(std::move(head));

    performance::GazeSample gaze;
    gaze.header = make_header(timestamp_ns, session_time_ns, source_id, source.gaze.confidence);
    gaze.channels = {
        {"x", source.gaze.x, source.gaze.confidence},
        {"y", source.gaze.y, source.gaze.confidence}
    };
    output.gaze.push_back(std::move(gaze));
    return output;
}

performance::RetargetedSkeletonSample retarget_to_engine(
    const performance::SkeletonSample& source,
    const bdfrpc::RetargetProfile& profile) {
    bdfrpc::SkeletonPose pose;
    pose.timestamp_ns = source.header.source_time_ns;
    pose.confidence = source.header.confidence;
    pose.translation = {source.root_translation.x, source.root_translation.y, source.root_translation.z};

    const bdfrpc::Quaternion root_q{
        source.root_rotation.w, source.root_rotation.x,
        source.root_rotation.y, source.root_rotation.z
    };
    const auto root_axis_angle = bdfrpc::quaternion_to_axis_angle(root_q);
    pose.global_orientation = {root_axis_angle.x, root_axis_angle.y, root_axis_angle.z};

    pose.joints.reserve(source.joints.size());
    for (const auto& source_joint : source.joints) {
        bdfrpc::JointPose joint;
        joint.name = source_joint.name;
        joint.parent = source_joint.parent_index;
        const bdfrpc::Quaternion q{
            source_joint.rotation.w, source_joint.rotation.x,
            source_joint.rotation.y, source_joint.rotation.z
        };
        joint.rotation = bdfrpc::quaternion_to_axis_angle(q);
        pose.joints.push_back(std::move(joint));
    }

    const auto retargeted = bdfrpc::retarget_skeleton(pose, profile);
    performance::RetargetedSkeletonSample output;
    output.header = source.header;
    output.profile_name = retargeted.profile_name;
    output.root_translation = {
        retargeted.root_translation[0],
        retargeted.root_translation[1],
        retargeted.root_translation[2]
    };
    output.root_rotation = to_engine_quaternion({
        retargeted.root_orientation[0],
        retargeted.root_orientation[1],
        retargeted.root_orientation[2]
    });
    output.bones.reserve(retargeted.bones.size());
    for (const auto& bone : retargeted.bones) {
        output.bones.push_back({
            bone.bone,
            to_engine_quaternion(bone.rotation)
        });
    }
    return output;
}

performance::HandSample to_engine_hand(
    const std::vector<float>& pca_coefficients,
    const performance::HandPcaBasis& basis,
    bool left_hand,
    performance::Nanoseconds source_time_ns,
    performance::Nanoseconds session_time_ns,
    const std::string& source_id,
    float confidence) {
    performance::HandSample output;
    if (pca_coefficients.size() != 6 || source_time_ns < 0 ||
        session_time_ns < 0 || source_id.empty() ||
        !std::isfinite(confidence) || confidence < 0.0F || confidence > 1.0F)
        return output;
    std::array<float, 6> coefficients{};
    std::copy(pca_coefficients.begin(), pca_coefficients.end(), coefficients.begin());
    const auto header = make_header(source_time_ns, session_time_ns, source_id, confidence);
    (void)performance::expand_hand_pca(coefficients, basis, left_hand, output, header);
    return output;
}

performance::SkeletonSample to_engine_skeleton(
    const bdfrpc::SkeletonPose& source,
    performance::Nanoseconds session_time_ns,
    const std::string& source_id) {
    performance::SkeletonSample body;
    body.header = make_header(source.timestamp_ns, session_time_ns, source_id, source.confidence);
    body.skeleton_profile = bdfrpc::EasyMocapSkeletonMapper::model_name(source.model);
    body.root_translation = {source.translation[0], source.translation[1], source.translation[2]};
    body.root_rotation = to_engine_quaternion(
        {source.global_orientation[0], source.global_orientation[1], source.global_orientation[2]});
    body.joints.reserve(source.joints.size());
    for (const auto& source_joint : source.joints) {
        performance::JointSample joint;
        joint.name = source_joint.name;
        joint.parent_index = source_joint.parent;
        joint.rotation = to_engine_quaternion(source_joint.rotation);
        joint.confidence = source.confidence;
        body.joints.push_back(std::move(joint));
    }

    const auto append_channels = [&body, &source](const std::vector<float>& values,
                                                   const std::string& prefix) {
        for (std::size_t i = 0; i < values.size(); ++i)
            body.channels.push_back({prefix + "_" + std::to_string(i), values[i], source.confidence});
    };
    append_channels(source.shape, "shape");
    append_channels(source.expression, "expression");
    append_channels(source.left_hand_pca, "left_hand_pca");
    append_channels(source.right_hand_pca, "right_hand_pca");
    return body;
}

performance::PerformanceFrame fuse_to_engine_frame(
    const bdfrpc::SyncedFrameSet& source,
    performance::Nanoseconds session_time_ns,
    bdfrpc::SourceRouter router) {
    bdfrpc::FusionCore fusion(std::move(router));
    return to_engine_frame(fusion.fuse(source), session_time_ns, "bdfrpc-fusion");
}

std::vector<performance::FaceCurve> mix_face_curves(
    const std::vector<performance::FaceCurve>& base,
    std::vector<bdfr::CurveLayer> layers) {
    bdfr::CurveMap base_map;
    for (const auto& curve : base) base_map[curve.name] = curve.value;
    const auto mixed = bdfr::CurveMixer::mix(base_map, std::move(layers));
    std::vector<performance::FaceCurve> output;
    output.reserve(mixed.size());
    for (const auto& item : mixed) output.push_back({item.first, item.second});
    return output;
}

performance::PerformanceFrame sample_emotion(
    bdfr::expression::Emotion emotion,
    float intensity,
    double time_seconds,
    performance::Nanoseconds session_start_ns,
    const std::string& source_id) {
    performance::PerformanceFrame output;
    output.session_time_ns = session_start_ns;
    if (!std::isfinite(time_seconds) || time_seconds < 0.0 ||
        !std::isfinite(intensity) || intensity < 0.0F ||
        session_start_ns < 0 || source_id.empty()) return output;
    const long double offset = static_cast<long double>(time_seconds) * 1000000000.0L;
    if (offset > static_cast<long double>(std::numeric_limits<performance::Nanoseconds>::max()))
        return output;
    const auto offset_ns = static_cast<performance::Nanoseconds>(std::llround(offset));
    if (session_start_ns > std::numeric_limits<performance::Nanoseconds>::max() - offset_ns)
        return output;
    const auto timestamp = session_start_ns + offset_ns;
    const float normalized = std::clamp(intensity, 0.0F, 1.0F);
    const auto curves = bdfr::expression::EmotionEngine::pose(emotion, normalized);

    performance::FaceSample face;
    face.header = make_header(offset_ns, timestamp, source_id, normalized);
    std::vector<std::pair<std::string, float>> ordered(curves.begin(), curves.end());
    std::sort(ordered.begin(), ordered.end(),
              [](const auto& a, const auto& b) { return a.first < b.first; });
    for (const auto& item : ordered) face.curves.push_back({item.first, item.second});
    output.faces.push_back(std::move(face));

    static constexpr const char* labels[] = {
        "neutral", "happiness", "sadness", "anger",
        "fear", "surprise", "disgust", "contempt"
    };
    const auto index = static_cast<std::size_t>(emotion);
    if (index >= std::size(labels)) return {};
    performance::EmotionSample sample;
    sample.header = make_header(offset_ns, timestamp, source_id, normalized);
    sample.label = labels[index];
    sample.intensity = normalized;
    output.emotions.push_back(std::move(sample));
    return output;
}

performance::PerformanceFrame sample_instant_event(
    const bdfr::expression::InstantEvent& event,
    double time_seconds,
    performance::Nanoseconds session_start_ns,
    const std::string& source_id) {
    performance::PerformanceFrame output;
    output.session_time_ns = session_start_ns;
    if (!std::isfinite(time_seconds) || time_seconds < 0.0 ||
        !std::isfinite(event.startSeconds) || !std::isfinite(event.durationSeconds) ||
        !std::isfinite(event.intensity) || event.startSeconds < 0.0 ||
        event.durationSeconds <= 0.0 || session_start_ns < 0 || source_id.empty())
        return output;
    const long double offset = static_cast<long double>(time_seconds) * 1000000000.0L;
    if (offset > static_cast<long double>(std::numeric_limits<performance::Nanoseconds>::max()))
        return output;
    const auto offset_ns = static_cast<performance::Nanoseconds>(std::llround(offset));
    if (session_start_ns > std::numeric_limits<performance::Nanoseconds>::max() - offset_ns)
        return output;
    const auto curves = bdfr::expression::InstantEventGenerator::sample(event, time_seconds);
    performance::FaceSample face;
    face.header = make_header(offset_ns, session_start_ns + offset_ns, source_id, 1.0F);
    std::vector<std::pair<std::string, float>> ordered(curves.begin(), curves.end());
    std::sort(ordered.begin(), ordered.end(),
              [](const auto& a, const auto& b) { return a.first < b.first; });
    for (const auto& item : ordered) face.curves.push_back({item.first, item.second});
    if (!face.curves.empty()) output.faces.push_back(std::move(face));
    return output;
}

performance::PerformanceFrame sample_procedural_face(
    const bdfr::expression::BehaviorProfile& profile,
    double time_seconds,
    performance::Nanoseconds session_start_ns,
    std::uint32_t personality_seed,
    const std::string& source_id) {
    performance::PerformanceFrame output;
    output.session_time_ns = session_start_ns;
    if (!std::isfinite(time_seconds) || time_seconds < 0.0 || source_id.empty())
        return output;
    auto facial = bdfr::expression::BehaviorEngine::sample(
        profile, time_seconds, personality_seed);
    facial.timestampSeconds = time_seconds;
    // BehaviorProfile head amplitudes are explicitly expressed in degrees;
    // canonical head-pose channels and quaternion conversion use radians.
    constexpr float degrees_to_radians = 0.01745329251994329577F;
    facial.head.pitch *= degrees_to_radians;
    facial.head.yaw *= degrees_to_radians;
    facial.head.roll *= degrees_to_radians;
    const long double offset = static_cast<long double>(time_seconds) * 1000000000.0L;
    if (offset > static_cast<long double>(std::numeric_limits<performance::Nanoseconds>::max()))
        return output;
    const auto offset_ns = static_cast<performance::Nanoseconds>(std::llround(offset));
    if (session_start_ns > std::numeric_limits<performance::Nanoseconds>::max() - offset_ns)
        return output;
    output = to_engine_frame(facial, session_start_ns + offset_ns, source_id);
    return output;
}

std::vector<performance::FaceCurve> sample_text_lipsync_curves(
    const std::string& text,
    double time_seconds,
    const bdfr::speech::TextSpeechOptions& options) {
    if (text.empty() || !std::isfinite(time_seconds) || time_seconds < 0.0) return {};
    const auto events = bdfr::speech::TextSpeechPlanner::plan(text, options);
    const auto curves = bdfr::speech::VisemeSynthesizer::sample(events, time_seconds);
    std::vector<performance::FaceCurve> output;
    output.reserve(curves.size());
    for (const auto& item : curves) output.push_back({item.first, item.second});
    std::sort(output.begin(), output.end(),
              [](const auto& a, const auto& b) { return a.name < b.name; });
    return output;
}

std::vector<performance::SpeechSample> plan_text_speech(
    const std::string& text,
    performance::Nanoseconds session_start_ns,
    const std::string& source_id,
    const bdfr::speech::TextSpeechOptions& options) {
    std::vector<performance::SpeechSample> output;
    if (text.empty() || source_id.empty()) return output;
    const auto events = bdfr::speech::TextSpeechPlanner::plan(text, options);
    output.reserve(events.size());
    for (const auto& event : events) {
        if (!std::isfinite(event.startSeconds) || !std::isfinite(event.durationSeconds) ||
            !std::isfinite(event.emphasis) || event.startSeconds < 0.0 ||
            event.durationSeconds < 0.0) continue;
        const long double offset = static_cast<long double>(event.startSeconds) * 1000000000.0L;
        if (offset > static_cast<long double>(std::numeric_limits<performance::Nanoseconds>::max()))
            continue;
        const auto offset_ns = static_cast<performance::Nanoseconds>(std::llround(offset));
        if (session_start_ns > std::numeric_limits<performance::Nanoseconds>::max() - offset_ns)
            continue;

        performance::SpeechSample sample;
        sample.header = make_header(offset_ns, session_start_ns + offset_ns,
                                    source_id, 1.0F);
        sample.text = event.token;
        // TextSpeechPlanner emits grapheme tokens, not acoustically recognized phonemes.
        sample.phoneme.clear();
        switch (event.viseme) {
        case bdfr::speech::Viseme::Rest: sample.viseme = "rest"; break;
        case bdfr::speech::Viseme::MBP: sample.viseme = "MBP"; break;
        case bdfr::speech::Viseme::FV: sample.viseme = "FV"; break;
        case bdfr::speech::Viseme::TH: sample.viseme = "TH"; break;
        case bdfr::speech::Viseme::L: sample.viseme = "L"; break;
        case bdfr::speech::Viseme::R: sample.viseme = "R"; break;
        case bdfr::speech::Viseme::WQ: sample.viseme = "WQ"; break;
        case bdfr::speech::Viseme::EE: sample.viseme = "EE"; break;
        case bdfr::speech::Viseme::AA: sample.viseme = "AA"; break;
        case bdfr::speech::Viseme::OH: sample.viseme = "OH"; break;
        case bdfr::speech::Viseme::CHSH: sample.viseme = "CHSH"; break;
        case bdfr::speech::Viseme::KNG: sample.viseme = "KNG"; break;
        case bdfr::speech::Viseme::SZTDN: sample.viseme = "SZTDN"; break;
        }
        sample.normalized_energy = std::clamp(event.emphasis, 0.0F, 1.0F);
        output.push_back(std::move(sample));
    }
    return output;
}

std::vector<performance::PerformanceFrame> to_engine_phoneme_frames(
    const std::vector<TimedPhoneme>& phonemes,
    const bdfr::speech::LanguageProfile& language,
    performance::Nanoseconds session_start_ns,
    const std::string& source_id) {
    std::vector<performance::PerformanceFrame> output;
    if (session_start_ns < 0 || source_id.empty()) return output;
    output.reserve(phonemes.size());

    const auto viseme_name = [](bdfr::speech::Viseme viseme) -> const char* {
        switch (viseme) {
        case bdfr::speech::Viseme::Rest: return "rest";
        case bdfr::speech::Viseme::MBP: return "MBP";
        case bdfr::speech::Viseme::FV: return "FV";
        case bdfr::speech::Viseme::TH: return "TH";
        case bdfr::speech::Viseme::L: return "L";
        case bdfr::speech::Viseme::R: return "R";
        case bdfr::speech::Viseme::WQ: return "WQ";
        case bdfr::speech::Viseme::EE: return "EE";
        case bdfr::speech::Viseme::AA: return "AA";
        case bdfr::speech::Viseme::OH: return "OH";
        case bdfr::speech::Viseme::CHSH: return "CHSH";
        case bdfr::speech::Viseme::KNG: return "KNG";
        case bdfr::speech::Viseme::SZTDN: return "SZTDN";
        }
        return "rest";
    };

    for (const auto& phoneme : phonemes) {
        if (phoneme.phoneme.empty() || !std::isfinite(phoneme.start_seconds) ||
            !std::isfinite(phoneme.duration_seconds) ||
            !std::isfinite(phoneme.confidence) || phoneme.start_seconds < 0.0 ||
            phoneme.duration_seconds <= 0.0) continue;
        const long double offset = static_cast<long double>(phoneme.start_seconds) * 1000000000.0L;
        if (offset > static_cast<long double>(std::numeric_limits<performance::Nanoseconds>::max()))
            continue;
        const auto offset_ns = static_cast<performance::Nanoseconds>(std::llround(offset));
        if (session_start_ns > std::numeric_limits<performance::Nanoseconds>::max() - offset_ns)
            continue;
        const auto timestamp = session_start_ns + offset_ns;
        const float confidence = std::clamp(phoneme.confidence, 0.0F, 1.0F);
        const auto viseme = language.mapPhoneme(phoneme.phoneme);

        performance::PerformanceFrame frame;
        frame.session_time_ns = timestamp;
        performance::SpeechSample speech;
        speech.header = make_header(offset_ns, timestamp, source_id, confidence);
        speech.phoneme = phoneme.phoneme;
        speech.viseme = viseme_name(viseme);
        speech.normalized_energy = confidence;
        frame.speech.push_back(std::move(speech));

        const auto pose = bdfr::speech::VisemeSynthesizer::pose(viseme, confidence);
        if (!pose.empty()) {
            performance::FaceSample face;
            face.header = make_header(offset_ns, timestamp, source_id, confidence);
            std::vector<std::pair<std::string, float>> ordered(pose.begin(), pose.end());
            std::sort(ordered.begin(), ordered.end(),
                      [](const auto& a, const auto& b) { return a.first < b.first; });
            for (const auto& item : ordered)
                face.curves.push_back({item.first, item.second});
            frame.faces.push_back(std::move(face));
        }
        output.push_back(std::move(frame));
    }
    return output;
}

std::vector<performance::PerformanceFrame> plan_audio_behavior(
    const bdfr::audio::AudioBuffer& source,
    performance::Nanoseconds session_start_ns,
    const std::string& source_id) {
    std::vector<performance::PerformanceFrame> output;
    if (!source.valid() || session_start_ns < 0 || source_id.empty()) return output;

    const auto features = bdfr::audio::AudioFeatures::analyze(source);
    const auto prosody = bdfr::audio::ProsodyAnalyzer::analyze(features);
    const auto cues = bdfr::audio::AudioBehaviorPlanner::plan(prosody);
    output.reserve(cues.size());

    for (const auto& cue : cues) {
        if (!std::isfinite(cue.startSeconds) || !std::isfinite(cue.durationSeconds) ||
            !std::isfinite(cue.strength) || cue.startSeconds < 0.0 ||
            cue.durationSeconds < 0.0) continue;
        const long double offset = static_cast<long double>(cue.startSeconds) * 1000000000.0L;
        if (offset > static_cast<long double>(std::numeric_limits<performance::Nanoseconds>::max()))
            continue;
        const auto offset_ns = static_cast<performance::Nanoseconds>(std::llround(offset));
        if (session_start_ns > std::numeric_limits<performance::Nanoseconds>::max() - offset_ns)
            continue;

        const float strength = std::clamp(cue.strength, 0.0F, 1.0F);
        const auto timestamp = session_start_ns + offset_ns;
        performance::PerformanceFrame frame;
        frame.session_time_ns = timestamp;

        if (cue.type == bdfr::audio::AudioBehaviorCueType::Blink) {
            performance::FaceSample face;
            face.header = make_header(offset_ns, timestamp, source_id, strength);
            face.curves = {{"eyeBlinkLeft", strength}, {"eyeBlinkRight", strength}};
            frame.faces.push_back(std::move(face));
        } else {
            performance::HeadSample head;
            head.header = make_header(offset_ns, timestamp, source_id, strength);
            switch (cue.type) {
            case bdfr::audio::AudioBehaviorCueType::HeadNod: {
                const float pitch_radians = 0.10F * strength;
                head.rotation = euler_xyz_to_quaternion(pitch_radians, 0.0F, 0.0F);
                head.channels.push_back({"pitch", pitch_radians, strength});
                head.channels.push_back({"audio_head_nod", strength, strength});
                break;
            }
            case bdfr::audio::AudioBehaviorCueType::Breath:
                head.channels.push_back({"breath_intensity", strength, strength});
                head.channels.push_back({"breath_duration_seconds",
                                         static_cast<float>(cue.durationSeconds), strength});
                break;
            case bdfr::audio::AudioBehaviorCueType::Emphasis:
                head.channels.push_back({"speech_emphasis", strength, strength});
                break;
            case bdfr::audio::AudioBehaviorCueType::Blink:
                break;
            }
            frame.heads.push_back(std::move(head));
        }
        output.push_back(std::move(frame));
    }
    return output;
}

performance::PerformanceFrame to_engine_frame(
    const bdfr::audio::AudioBuffer& source,
    performance::Nanoseconds session_time_ns,
    const std::string& source_id) {
    performance::PerformanceFrame output;
    output.session_time_ns = session_time_ns;
    if (!source.valid()) return output;

    const auto features = bdfr::audio::AudioFeatures::analyze(source);
    const auto prosody = bdfr::audio::ProsodyAnalyzer::analyze(features);
    const auto pitch = bdfr::audio::PitchTracker::analyze(source);

    for (std::size_t i = 0; i < features.size(); ++i) {
        const auto& feature = features[i];
        const bdfr::audio::ProsodyFrame* prosody_frame =
            i < prosody.size() ? &prosody[i] : nullptr;
        const bdfr::audio::PitchFrame* pitch_frame =
            i < pitch.size() ? &pitch[i] : nullptr;

        performance::SpeechSample speech;
        const auto source_ns = static_cast<performance::Nanoseconds>(
            std::llround(feature.startSeconds * 1000000000.0));
        speech.header = make_header(source_ns, session_time_ns + source_ns,
                                    source_id,
                                    pitch_frame && pitch_frame->voiced
                                        ? pitch_frame->confidence
                                        : (prosody_frame && prosody_frame->speechActive ? 0.5F : 0.0F));
        speech.normalized_energy = prosody_frame
            ? std::clamp(prosody_frame->normalizedEnergy, 0.0F, 1.0F) : 0.0F;
        speech.pitch_hz = pitch_frame && pitch_frame->voiced
            ? pitch_frame->frequencyHz : 0.0F;
        output.speech.push_back(std::move(speech));
    }
    return output;
}

performance::PerformanceFrame to_engine_frame(
    const bdfrpc::FusedPerformanceFrame& source,
    performance::Nanoseconds session_time_ns,
    const std::string& source_id) {
    performance::PerformanceFrame output;
    output.session_time_ns = session_time_ns;

    for (const auto& sample : source.samples) {
        if (!sample.channels_valid()) continue;
        switch (sample.domain) {
        case bdfrpc::Domain::Face: {
            performance::FaceSample face;
            face.header = make_header(source, session_time_ns, source_id, sample.confidence);
            face.curves.reserve(sample.values.size());
            for (std::size_t i = 0; i < sample.values.size(); ++i)
                face.curves.push_back({channel_name(sample, i), sample.values[i]});
            output.faces.push_back(std::move(face));
            break;
        }
        case bdfrpc::Domain::Head: {
            performance::HeadSample head;
            head.header = make_header(source, session_time_ns, source_id, sample.confidence);
            head.channels = copy_channels(sample);
            output.heads.push_back(std::move(head));
            break;
        }
        case bdfrpc::Domain::Body: {
            performance::SkeletonSample body;
            body.header = make_header(source, session_time_ns, source_id, sample.confidence);
            body.skeleton_profile = "source-defined";
            body.channels = copy_channels(sample);
            output.bodies.push_back(std::move(body));
            break;
        }
        case bdfrpc::Domain::LeftHand: {
            performance::HandSample hand;
            hand.header = make_header(source, session_time_ns, source_id, sample.confidence);
            hand.channels = copy_channels(sample);
            output.left_hands.push_back(std::move(hand));
            break;
        }
        case bdfrpc::Domain::RightHand: {
            performance::HandSample hand;
            hand.header = make_header(source, session_time_ns, source_id, sample.confidence);
            hand.channels = copy_channels(sample);
            output.right_hands.push_back(std::move(hand));
            break;
        }
        }
    }
    return output;
}

} // namespace bdfr::integration
