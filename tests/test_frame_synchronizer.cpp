#include "bdfrpc/FrameSynchronizer.h"

#include <cassert>
#include <string>
#include <vector>

using namespace bdfrpc;

static CaptureFrame make_frame(const std::string& stream, TimestampNs timestamp,
                               std::uint64_t sequence) {
    CaptureFrame frame;
    frame.source_id = stream;
    frame.stream_id = stream;
    frame.timestamp_ns = timestamp;
    frame.sequence = sequence;
    return frame;
}

int main() {
    FrameSynchronizer sync(10, 4);
    sync.set_required_streams({"cam-a", "cam-b"});

    sync.push(make_frame("cam-a", 100, 1));
    assert(!sync.try_pop().has_value());
    sync.push(make_frame("cam-b", 106, 1));

    auto first = sync.try_pop();
    assert(first.has_value());
    assert(first->frames.size() == 2);
    assert(first->timestamp_ns == 106);
    assert(first->max_skew_ns == 6);
    assert(first->frames[0].stream_id == "cam-a");
    assert(first->frames[1].stream_id == "cam-b");
    assert(sync.stats().emitted_sets == 1);

    // A stale frame must be discarded once the lagging stream has a newer
    // queued frame, allowing the next synchronized pair to make progress.
    sync.push(make_frame("cam-a", 200, 2));
    sync.push(make_frame("cam-a", 300, 3));
    sync.push(make_frame("cam-b", 300, 2));
    auto second = sync.try_pop();
    assert(second.has_value());
    assert(second->timestamp_ns == 300);
    assert(second->max_skew_ns == 0);
    assert(sync.stats().streams.at("cam-a").dropped_stale == 1);
    assert(sync.stats().emitted_sets == 2);

    sync.set_tolerance_ns(0);
    sync.push(make_frame("cam-a", 400, 4));
    sync.push(make_frame("cam-b", 401, 3));
    assert(!sync.try_pop().has_value());

    sync.clear();
    assert(!sync.try_pop().has_value());
    return 0;
}
