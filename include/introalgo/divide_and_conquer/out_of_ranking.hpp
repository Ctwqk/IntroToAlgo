#pragma once

#include <cstddef>
#include <vector>

namespace introalgo {

/// Divide-and-conquer inversion counting, used to measure how different two
/// rankings are ("out of ranking" / user similarity).
///
/// For a ranking `RankA` (the position of each item in the *other* user's
/// ranking), the number of inversions -- pairs `i < j` with `RankA[i] > RankA[j]`
/// -- is the number of item pairs on which the two users disagree. It is
/// computed with a merge sort that counts, for every element taken from the
/// right half, how many elements of the left half are still waiting.
///
/// Time O(n log n), extra space O(n).
class OutOfRanking {
public:
    OutOfRanking() = default;

    /// Returns the number of inversions in `RankA` and leaves `RankA` sorted
    /// (the merge sort works in place on the vector that is passed in).
    /// The counter is reset on every call, so one object can be reused.
    long long user_similarity(std::vector<int>& RankA);

private:
    long long count = 0;

    /// Sorts nums[lo, hi) using `ext` as scratch space and adds the inversions
    /// found to `count`.
    void auxilary_merge(std::vector<int>& nums, std::vector<int>& ext, std::size_t lo,
                        std::size_t hi);
};

}  // namespace introalgo
