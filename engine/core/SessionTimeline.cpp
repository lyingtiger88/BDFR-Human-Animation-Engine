#include "bdfr/performance/SessionTimeline.h"

#include "bdfr/performance/SessionCodec.h"
#include "bdfr/performance/Validation.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <iterator>
#include <utility>

namespace bdfr::performance {

bool SessionRecorder::begin(std::string session_id) {
    if (session_id.empty()) return false;
    session_ = Session{};
    session_.session_id = std::move(session_id);
    active_ = true;
    return true;
}

bool SessionRecorder::add_actor(Actor actor) {
    if (!active_ || actor.id.empty()) return false;
    const auto duplicate = std::find_if(session_.actors.begin(), session_.actors.end(),
        [&actor](const Actor& existing) { return existing.id == actor.id; });
    if (duplicate != session_.actors.end()) return false;
    session_.actors.push_back(std::move(actor));
    return true;
}

bool SessionRecorder::append(PerformanceFrame frame) {
    if (!active_ || frame.session_time_ns < 0) return false;
    if (!session_.frames.empty() &&
        frame.session_time_ns < session_.frames.back().session_time_ns) return false;
    if (!validate_frame(frame).ok) return false;
    session_.frames.push_back(std::move(frame));
    return true;
}

std::vector<std::uint8_t> SessionRecorder::encode() const {
    if (!active_) return {};
    return serialize_session(session_);
}

void SessionRecorder::reset() noexcept {
    session_ = Session{};
    active_ = false;
}

bool SessionPlayer::open(std::span<const std::uint8_t> archive, std::string* error) {
    Session candidate;
    if (!deserialize_session(archive, candidate, error)) return false;
    session_ = std::move(candidate);
    frame_index_ = 0;
    position_ns_ = session_.frames.empty() ? 0 : session_.frames.front().session_time_ns;
    loaded_ = true;
    return true;
}

bool SessionPlayer::seek(Nanoseconds session_time_ns) noexcept {
    if (!loaded_ || session_.frames.empty() || session_time_ns < 0) return false;
    position_ns_ = session_time_ns;
    const auto it = std::upper_bound(session_.frames.begin(), session_.frames.end(), position_ns_,
        [](Nanoseconds time, const PerformanceFrame& frame) {
            return time < frame.session_time_ns;
        });
    frame_index_ = it == session_.frames.begin()
        ? 0 : static_cast<std::size_t>(std::distance(session_.frames.begin(), it) - 1);
    return true;
}

const PerformanceFrame* SessionPlayer::current() const noexcept {
    if (!loaded_ || session_.frames.empty() || frame_index_ >= session_.frames.size())
        return nullptr;
    return &session_.frames[frame_index_];
}

const PerformanceFrame* SessionPlayer::advance(
    Nanoseconds delta_ns, double playback_rate) noexcept {
    if (!loaded_ || delta_ns < 0 || !std::isfinite(playback_rate) || playback_rate < 0.0)
        return current();
    const long double scaled = static_cast<long double>(delta_ns) * playback_rate;
    if (scaled > static_cast<long double>(std::numeric_limits<Nanoseconds>::max()))
        return current();
    const auto increment = static_cast<Nanoseconds>(scaled);
    if (position_ns_ > std::numeric_limits<Nanoseconds>::max() - increment)
        return current();
    const Nanoseconds last = session_.frames.empty()
        ? 0 : session_.frames.back().session_time_ns;
    position_ns_ = std::min(position_ns_ + increment, last);
    (void)seek(position_ns_);
    return current();
}

void SessionPlayer::reset() noexcept {
    session_ = Session{};
    frame_index_ = 0;
    position_ns_ = 0;
    loaded_ = false;
}

} // namespace bdfr::performance
