#pragma once
#include "bdfr/performance/CanonicalSession.h"
#include <string>

namespace bdfr::performance {
struct ValidationResult {
    bool ok{true};
    std::string error;
};
ValidationResult validate(const Session& session);
}
