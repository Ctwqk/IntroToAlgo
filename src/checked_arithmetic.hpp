#pragma once

#include <limits>
#include <stdexcept>

namespace introalgo::detail {

inline long long checkedAdd(long long a, long long b) {
    constexpr auto lo = std::numeric_limits<long long>::min();
    constexpr auto hi = std::numeric_limits<long long>::max();
    if ((b > 0 && a > hi - b) || (b < 0 && a < lo - b))
        throw std::overflow_error("integer addition exceeds long long range");
    return a + b;
}

inline long long checkedSubtract(long long a, long long b) {
    constexpr auto lo = std::numeric_limits<long long>::min();
    constexpr auto hi = std::numeric_limits<long long>::max();
    if ((b > 0 && a < lo + b) || (b < 0 && a > hi + b))
        throw std::overflow_error("integer subtraction exceeds long long range");
    return a - b;
}

inline long long checkedMultiply(long long a, long long b) {
    constexpr auto lo = std::numeric_limits<long long>::min();
    constexpr auto hi = std::numeric_limits<long long>::max();
    if ((a > 0 && ((b > 0 && a > hi / b) || (b < 0 && b < lo / a))) ||
        (a < 0 && ((b > 0 && a < lo / b) || (b < 0 && a < hi / b))))
        throw std::overflow_error("integer multiplication exceeds long long range");
    return a * b;
}

}  // namespace introalgo::detail
