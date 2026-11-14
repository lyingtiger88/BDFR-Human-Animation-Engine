#pragma once
#include <array>
#include <cstdint>
#include <string>
#include <vector>

namespace bdfr::performance {

inline constexpr std::uint32_t schema_major = 1;
inline constexpr std::uint32_t schema_minor = 0;
using Nanoseconds = std::int64_t;
using ActorId = std::string;
using TrackId = std::string;

struct Vec3f { float x{}, y{}, z{}; };
struct Quaternionf { float x{}, y{}, z{}, w{1.0F}; };

enum class CoordinateSpace : std::uint8_t { Unknown, World, Camera, ActorRoot, LocalJoint };
enum class Domain : std::uint8_t { Face, Head, Gaze, Body, LeftHand, RightHand, Audio, Speech, Emotion, Event };

struct SampleHeader {
    Nanoseconds source_time_ns{};
    Nanoseconds session_time_ns{};
    float confidence{1.0F};
    bool valid{true};
    CoordinateSpace space{CoordinateSpace::Unknown};
    std::string source_id;
};

struct ScalarChannel {\n    std::string name;\n    float value{};\n    float confidence{1.0F};\n};\nstruct FaceCurve {
    std::string name;
    float value{};
};
struct FaceSample {
    SampleHeader header;
    std::vector<FaceCurve> curves;
    Vec3f head_translation{};
    Quaternionf head_rotation{};
    Vec3f gaze_direction{};
};
struct JointSample {
    std::string name;
    Vec3f translation{};
    Quaternionf rotation{};
    float confidence{1.0F};
};
struct SkeletonSample {
    SampleHeader header;
    std::string skeleton_profile;
    std::vector<JointSample> joints;
    Vec3f root_translation{};
    Quaternionf root_rotation{};
};
struct HandSample {
    SampleHeader header;
    std::array<JointSample, 25> joints{};\n    std::vector<ScalarChannel> channels;
    bool has_palm_pose{false};
    Vec3f palm_position{};
    Quaternionf palm_rotation{};
};
struct SpeechSample {
    SampleHeader header;
    std::string text;
    std::string phoneme;
    std::string viseme;
    float normalized_energy{};
    float pitch_hz{};
};
struct EmotionSample {
    SampleHeader header;
    std::string label;
    float intensity{};
};
struct PerformanceFrame {
    Nanoseconds session_time_ns{};
    std::vector<FaceSample> faces;
    std::vector<SkeletonSample> bodies;
    std::vector<HandSample> left_hands;
    std::vector<HandSample> right_hands;
    std::vector<SpeechSample> speech;
    std::vector<EmotionSample> emotions;
};
struct Actor {
    ActorId id;
    std::string display_name;
};
struct Session {
    std::uint32_t schema_version_major{schema_major};
    std::uint32_t schema_version_minor{schema_minor};
    std::string session_id;
    std::string timebase{"nanoseconds"};
    std::vector<Actor> actors;
    std::vector<PerformanceFrame> frames;
};

} // namespace bdfr::performance
