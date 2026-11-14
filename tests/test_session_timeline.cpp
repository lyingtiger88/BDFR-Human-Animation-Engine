#include "bdfr/performance/SessionTimeline.h"

#include <cassert>
#include <string>

using namespace bdfr::performance;

int main() {
    SessionRecorder recorder;
    assert(!recorder.begin(""));
    assert(recorder.begin("timeline-session"));
    assert(recorder.add_actor({"actor-1", "Performer"}));
    assert(!recorder.add_actor({"actor-1", "Duplicate"}));

    PerformanceFrame first;
    first.session_time_ns = 0;
    PerformanceFrame second;
    second.session_time_ns = 100;
    PerformanceFrame third;
    third.session_time_ns = 200;
    assert(recorder.append(first));
    assert(recorder.append(second));
    assert(recorder.append(third));

    PerformanceFrame out_of_order;
    out_of_order.session_time_ns = 150;
    assert(!recorder.append(out_of_order));

    const auto archive = recorder.encode();
    assert(!archive.empty());

    SessionPlayer player;
    std::string error;
    assert(player.open(archive, &error));
    assert(error.empty());
    assert(player.loaded());
    assert(player.current() != nullptr);
    assert(player.current()->session_time_ns == 0);

    assert(player.seek(150));
    assert(player.current()->session_time_ns == 100);
    assert(player.advance(50, 2.0) != nullptr);
    assert(player.position() == 200);
    assert(player.current()->session_time_ns == 200);
    assert(player.advance(100, 1.0)->session_time_ns == 200);
    assert(!player.seek(-1));

    player.reset();
    assert(!player.loaded());
    assert(player.current() == nullptr);
    recorder.reset();
    assert(recorder.encode().empty());
    return 0;
}
