#include "introalgo/strings/rolling_hash.hpp"

#include <stdexcept>
#include <utility>

namespace introalgo {

namespace {

int code(char c) {
    switch (c) {
        case 'A':
            return 0;
        case 'C':
            return 1;
        case 'G':
            return 2;
        case 'T':
            return 3;
    }
    throw std::invalid_argument(std::string("DNAMatch: not a DNA letter: '") + c + "'");
}

}  // namespace

DNAMatch::DNAMatch(std::string s1, std::string s2, long long m)
    : pattern(std::move(s1)), target(std::move(s2)), mod(m) {
    if (mod < 2 || mod > (1LL << 40)) throw std::invalid_argument("DNAMatch: bad modulus");
    for (char c : target) code(c);  // validates the text
    for (char c : pattern) p = ((p << 2) + code(c)) % mod;
}

bool DNAMatch::isMatch(std::size_t idx) const {
    std::size_t i = 0;
    for (; i < pattern.size(); i++)
        if (pattern[i] != target[idx - pattern.size() + i]) break;
    return i == pattern.size();
}

std::vector<int> DNAMatch::match() const {
    std::vector<int> matches;
    const std::size_t m = pattern.size();
    if (m == 0 || m > target.size()) return matches;

    // 4^(m-1) mod M: the weight of the digit that leaves the window.  (The original shifted an
    // int by 2m-2 bits, which overflows for patterns longer than 16 letters.)
    long long highPow = 1;
    for (std::size_t k = 1; k < m; k++) highPow = (highPow << 2) % mod;

    long long tmp = 0;
    std::size_t idx = 0;
    for (; idx < m; idx++) tmp = ((tmp << 2) + code(target[idx])) % mod;
    if (tmp == p && isMatch(idx)) matches.push_back(static_cast<int>(idx - m));
    while (idx < target.size()) {
        const long long swa = code(target[idx - m]) * highPow % mod;
        // Reduce again after adding the new digit; the original compared an unreduced value
        // and could miss a match.
        tmp = ((((tmp - swa + mod) % mod) << 2) + code(target[idx++])) % mod;
        if (tmp == p && isMatch(idx)) matches.push_back(static_cast<int>(idx - m));
    }
    return matches;
}

}  // namespace introalgo
