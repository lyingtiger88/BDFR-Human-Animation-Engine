#include "bdfr/core/Platform.h"
#include <cassert>
int main() {
    static_assert(sizeof(void*) == 8);
    assert(bdfr::core::PlatformInfo::is_64_bit);
    assert(bdfr::core::PlatformInfo::pointer_bytes == 8);
    return 0;
}
