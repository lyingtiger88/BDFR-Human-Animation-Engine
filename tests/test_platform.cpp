#include "bdfr/core/Platform.h"
#include "bdfr/performance/Validation.h"

#include <cassert>
#include <limits>

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

    bdfr::performance::FaceSample face;
    face.header = {100, 1000000, 0.9F, true,
                   bdfr::performance::CoordinateSpace::Unknown, "camera-1"};
    face.curves.push_back({"jawOpen", 0.5F});
    session.frames[1].faces.push_back(face);
    assert(bdfr::performance::validate(session).ok);

    session.frames[1].faces.back().header.confidence = 1.5F;
    assert(!bdfr::performance::validate(session).ok);
    session.frames[1].faces.back().header.confidence = 0.9F;

    bdfr::performance::HeadSample head;
    head.header = {100, 1000000, 0.8F, true,
                   bdfr::performance::CoordinateSpace::Unknown, "camera-1"};
    head.channels.push_back({"yaw", 0.1F, 0.8F});
    session.frames[1].heads.push_back(head);
    assert(bdfr::performance::validate(session).ok);
    session.frames[1].heads.back().channels[0].value =
        std::numeric_limits<float>::quiet_NaN();
    assert(!bdfr::performance::validate(session).ok);
    return 0;
}
