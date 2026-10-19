#pragma once

// Small helpers shared by the randomized (property-based) tests.
// All tests use fixed seeds so that any failure is reproducible.

#include <cstdint>
#include <random>
#include <string>
#include <vector>

namespace testutil {

using Rng = std::mt19937_64;

inline int randInt(Rng& rng, int lo, int hi) {
    return std::uniform_int_distribution<int>(lo, hi)(rng);
}

inline double randReal(Rng& rng, double lo, double hi) {
    return std::uniform_real_distribution<double>(lo, hi)(rng);
}

inline std::vector<int> randVector(Rng& rng, std::size_t n, int lo, int hi) {
    std::vector<int> v(n);
    for (auto& x : v) x = randInt(rng, lo, hi);
    return v;
}

inline std::string randString(Rng& rng, std::size_t n, const std::string& alphabet) {
    std::string s(n, ' ');
    for (auto& c : s)
        c = alphabet[static_cast<std::size_t>(randInt(rng, 0, int(alphabet.size()) - 1))];
    return s;
}

}  // namespace testutil
