#include "bdfr/core/Platform.h"
static_assert(sizeof(void*) == 8, "BDFR engine core must be compiled for a 64-bit target.");
static_assert(bdfr::core::PlatformInfo::is_64_bit);
