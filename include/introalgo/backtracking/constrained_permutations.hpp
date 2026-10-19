#pragma once

#include <cstddef>
#include <iosfwd>
#include <utility>
#include <vector>

namespace introalgo {

/// Enumerates the permutations of 1..n whose longest increasing subsequence has at most `shd1`
/// elements and whose longest decreasing subsequence has at most `shd2` elements (the original
/// class `numbers`; with n = 9, 3 and 3 these are the 1764 permutations of the Erdos-Szekeres
/// extremal case).
///
/// The search extends partial permutations position by position, level by level, and prunes a
/// partial permutation as soon as one of its two subsequence lengths exceeds its threshold.
/// For every position the pair (longest increasing, longest decreasing) subsequence ending there
/// is maintained incrementally.
class ConstrainedPermutations {
public:
    /// Throws std::invalid_argument for n < 0 or n > 12.
    ConstrainedPermutations(int numbers, int increasing, int decreasing);

    /// Upper bound on the number of partial permutations kept at one level.
    static constexpr std::size_t kMaxStates = 4000000;

    /// Runs the search (calling it again recomputes the same answer).  Throws std::length_error if
    /// the pruning is too weak and more than kMaxStates partial permutations would be needed.
    void find();

    /// All valid permutations in lexicographic order of the search (increasing).
    const std::vector<std::vector<int>>& answers() const { return ans; }

    /// Prints the question, up to 100 answers and their number, like the original.
    void show(std::ostream& os) const;

private:
    std::vector<std::vector<int>> ans;
    int n;
    int shd1;  // increasing threshold
    int shd2;  // decreasing threshold

    bool judge(const std::vector<int>& stage, int idx, std::vector<std::pair<int, int>>& id) const;
};

}  // namespace introalgo
