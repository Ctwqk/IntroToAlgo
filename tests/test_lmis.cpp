#include <gtest/gtest.h>

#include <algorithm>
#include <functional>
#include <vector>

#include "introalgo/dynamic_programming/lmis.hpp"
#include "test_utils.hpp"

using introalgo::Lmis;

namespace {

// Exhaustive oracle over all 2^n subsequences.
int bruteForce(const std::vector<int>& v) {
    int best = 0;
    for (unsigned mask = 0; mask < (1u << v.size()); ++mask) {
        int count = 0;
        bool ok = true;
        int last = 0;
        for (std::size_t i = 0; i < v.size() && ok; ++i) {
            if (!(mask & (1u << i))) continue;
            if (count > 0 && v[i] <= last) ok = false;
            last = v[i];
            ++count;
        }
        if (ok) best = std::max(best, count);
    }
    return best;
}

}  // namespace

TEST(Lmis, KnownAnswers) {
    EXPECT_EQ(Lmis({10, 9, 2, 5, 3, 7, 101, 18}).findLMIS(), 4);
    EXPECT_EQ(Lmis({1, 2, 3, 4, 5}).findLMIS(), 5);
    EXPECT_EQ(Lmis({5, 4, 3, 2, 1}).findLMIS(), 1);
    EXPECT_EQ(Lmis({7, 7, 7}).findLMIS(), 1);  // strictly increasing
    EXPECT_EQ(Lmis({}).findLMIS(), 0);
}

TEST(Lmis, RunningMaximumIsResetForEveryPosition) {
    // The original carried `max` over from the previous position: after 1 2 3 the value 0 was
    // credited with a length of 3 and the answer for this input came out as 4.
    EXPECT_EQ(Lmis({1, 2, 3, 0, 1}).findLMIS(), 3);
}

TEST(Lmis, SubsequenceIsValid) {
    Lmis l({10, 9, 2, 5, 3, 7, 101, 18});
    const auto sub = l.subsequence();
    EXPECT_EQ(sub.size(), 4u);
    EXPECT_EQ(std::adjacent_find(sub.begin(), sub.end(), std::greater_equal<int>()), sub.end());
    EXPECT_TRUE(Lmis({}).subsequence().empty());
}

TEST(Lmis, RandomizedMatchesExhaustiveSearch) {
    testutil::Rng rng(4);
    for (int iter = 0; iter < 400; ++iter) {
        auto v = testutil::randVector(rng, std::size_t(testutil::randInt(rng, 0, 13)), -5, 5);
        Lmis l(v);
        EXPECT_EQ(l.findLMIS(), bruteForce(v));
        // The reported subsequence is strictly increasing, in order, and of optimal length.
        const auto sub = l.subsequence();
        EXPECT_EQ(static_cast<int>(sub.size()), bruteForce(v));
        EXPECT_EQ(std::adjacent_find(sub.begin(), sub.end(), std::greater_equal<int>()), sub.end());
        std::size_t k = 0;
        for (int x : v)
            if (k < sub.size() && sub[k] == x) ++k;
        EXPECT_EQ(k, sub.size());
    }
}
