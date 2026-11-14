#pragma once

#include "bdfr/audio/WavAudio.h"

#include <string>
#include <vector>

namespace bdfr::speech {

struct AlignedPhoneme {
    std::string symbol;
    double start_seconds{0.0};
    double duration_seconds{0.0};
    float confidence{1.0F};
};

struct SpeechRecognitionResult {
    std::string transcript;
    std::vector<AlignedPhoneme> phonemes;
    float confidence{0.0F};
};

// Provider boundary for Whisper/Vosk/NeMo or a forced-alignment backend.
// Implementations own model loading, licensing and inference dependencies;
// the canonical engine remains independent of any particular ASR runtime.
class SpeechRecognitionProvider {
public:
    virtual ~SpeechRecognitionProvider() = default;
    virtual bool transcribe_and_align(
        const audio::AudioBuffer& audio,
        SpeechRecognitionResult& result,
        std::string* error = nullptr) const = 0;
};

} // namespace bdfr::speech
