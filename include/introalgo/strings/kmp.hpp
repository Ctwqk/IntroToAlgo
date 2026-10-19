#pragma once

#include <string>
#include <vector>

namespace introalgo {

/// Knuth-Morris-Pratt substring search: every (possibly overlapping) occurrence of
/// `pattern` in `target` in O(n + m) time and O(m) extra space.
class KMP {
public:
    KMP(std::string s1, std::string s2);

    /// Start positions of all occurrences, in increasing order.  An empty pattern has no matches.
    std::vector<int> calculateKMP() const;

    /// The prefix function: lps()[i] is the length of the longest proper prefix of
    /// pattern[0..i] that is also a suffix of it.
    const std::vector<int>& lps() const { return lps_; }

private:
    std::string target;
    std::string pattern;
    std::vector<int> lps_;
    int n, m;       // pattern / target length
    void getLPS();  // calculates the prefix lengths
};

}  // namespace introalgo
