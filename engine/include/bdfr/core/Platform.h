#pragma once
#include <cstddef>
#include <string_view>

namespace bdfr::core {
struct PlatformInfo {
    static constexpr std::size_t pointer_bytes = sizeof(void*);
    static constexpr bool is_64_bit = pointer_bytes == 8;
    static constexpr std::string_view product = "BDFR Human Animation Engine";
};
}
