#pragma once

#include "bdfr/performance/CanonicalSession.h"

#include <cstdint>
#include <span>
#include <string>
#include <vector>

namespace bdfr::performance {

// Deterministic, versioned, little-endian binary archive for canonical sessions.
// Deserialization is bounded and validates the complete decoded session.
std::vector<std::uint8_t> serialize_session(const Session& session);
bool deserialize_session(
    std::span<const std::uint8_t> bytes,
    Session& output,
    std::string* error = nullptr);

} // namespace bdfr::performance
