#pragma once

#include <string>
#include <vector>

namespace introalgo {

/// Rabin-Karp matching of a DNA pattern (alphabet A, C, G, T) in a DNA text.
///
/// The four letters are digits 0..3 of a base-4 number; the hash of every window of the text
/// is obtained from the previous one in O(1) by removing the leading digit and appending the
/// next one (the original comment: "use binary to simulate quaternary").  Hash hits are
/// verified character by character, so the result is exact for every modulus.
///
/// Expected O(n + m) time, worst case O(nm) when many windows collide.
class DNAMatch {
public:
    /// The prime used by the original program (11! + 1).
    static constexpr long long kDefaultMod = 39916801;

    /// Throws std::invalid_argument for letters other than A, C, G, T or for a modulus
    /// outside [2, 2^40].  `mod` is exposed so tests can force hash collisions.
    DNAMatch(std::string pattern, std::string target, long long mod = kDefaultMod);

    /// Start positions of every (possibly overlapping) occurrence, in increasing order.
    std::vector<int> match() const;

private:
    std::string pattern, target;
    long long mod;
    long long p = 0;  // hash of the pattern

    bool isMatch(std::size_t idx) const;  // does the window ending before idx equal the pattern?
};

}  // namespace introalgo
