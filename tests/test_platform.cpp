#include "bdfr/core/Platform.h"
#include "bdfr/performance/Validation.h"
#include <cassert>
int main() {
    static_assert(sizeof(void*) == 8);
    assert(bdfr::core::PlatformInfo::is_64_bit);
    assert(bdfr::core::PlatformInfo::pointer_bytes == 8);
    bdfr::performance::Session session;
    session.session_id = "test-session";
    session.actors.push_back({"actor-1", "Test Actor"});
    session.frames.push_back({0});
    session.frames.push_back({1000000});
    assert(bdfr::performance::validate(session).ok);
    session.frames[1].session_time_ns = -1;
    assert(!bdfr::performance::validate(session).ok);
    session.frames[1].session_time_ns = 1000000;
    session.frames[1].faces.push_back({});
    session.frames[1].faces.back().header.confidence = 1.5F;
    assert(!bdfr::performance::validate(session).ok);
    return 0;
}
