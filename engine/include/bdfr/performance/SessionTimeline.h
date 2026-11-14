#pragma once

#include "bdfr/performance/CanonicalSession.h"

#include <cstddef>
#include <cstdint>
#include <span>
#include <string>
#include <vector>

namespace bdfr::performance {

class SessionRecorder {
public:
    bool begin(std::string session_id);
    bool add_actor(Actor actor);
    bool append(PerformanceFrame frame);
    const Session& session() const noexcept { return session_; }
    std::vector<std::uint8_t> encode() const;
    void reset() noexcept;

private:
    Session session_;
    bool active_{false};
};

class SessionPlayer {
public:
    bool open(std::span<const std::uint8_t> archive, std::string* error = nullptr);
    bool seek(Nanoseconds session_time_ns) noexcept;
    const PerformanceFrame* current() const noexcept;
    const PerformanceFrame* advance(Nanoseconds delta_ns, double playback_rate = 1.0) noexcept;
    void reset() noexcept;
    Nanoseconds position() const noexcept { return position_ns_; }
    bool loaded() const noexcept { return loaded_; }

private:
    Session session_;
    std::size_t frame_index_{0};
    Nanoseconds position_ns_{0};
    bool loaded_{false};
};

} // namespace bdfr::performance
