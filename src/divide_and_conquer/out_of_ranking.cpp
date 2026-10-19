#include "introalgo/divide_and_conquer/out_of_ranking.hpp"

namespace introalgo {

long long OutOfRanking::user_similarity(std::vector<int>& RankA) {
    count = 0;
    std::vector<int> ext(RankA.size());
    auxilary_merge(RankA, ext, 0, RankA.size());
    return count;
}

void OutOfRanking::auxilary_merge(std::vector<int>& nums, std::vector<int>& ext, std::size_t lo,
                                  std::size_t hi) {
    if (hi - lo < 2) return;
    const std::size_t mid = lo + (hi - lo) / 2;
    auxilary_merge(nums, ext, lo, mid);
    auxilary_merge(nums, ext, mid, hi);

    std::size_t l = lo, r = mid, idx = lo;
    while (l < mid && r < hi) {
        // Equal values are not an inversion, so the left element goes first on ties.
        while (l < mid && nums[l] <= nums[r]) ext[idx++] = nums[l++];
        // Every element still waiting on the left is greater than nums[r].
        count += static_cast<long long>(mid - l);
        ext[idx++] = nums[r++];
    }
    while (l < mid) ext[idx++] = nums[l++];
    while (r < hi) ext[idx++] = nums[r++];
    for (std::size_t k = lo; k < hi; ++k) nums[k] = ext[k];
}

}  // namespace introalgo
