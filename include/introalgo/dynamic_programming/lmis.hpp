#pragma once

#include <vector>

namespace introalgo {

/// Longest monotonically (strictly) increasing subsequence, O(n^2) dynamic
/// program: ans[i] is the length of the longest increasing subsequence that
/// ends at position i.
class Lmis {
public:
    explicit Lmis(std::vector<int> s);

    /// Length of the longest strictly increasing subsequence (0 for an empty input).
    int findLMIS();

    /// One longest strictly increasing subsequence (valid after findLMIS()).
    std::vector<int> subsequence();

private:
    std::vector<int> sequence;
    std::vector<int> ans;
    std::vector<int> prev;
};

}  // namespace introalgo
