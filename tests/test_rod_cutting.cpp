#include <gtest/gtest.h>

#include <algorithm>
#include <limits>
#include <numeric>
#include <vector>

#include "introalgo/dynamic_programming/rod_cutting.hpp"
#include "test_utils.hpp"

using introalgo::RodCutting;

namespace {

// Exhaustive oracle: try every first piece, recursively (2^(n-1) compositions of n).
int bruteForce(const std::vector<int>& price, int n) {
    if (n == 0) return 0;
    int best = price[0] + bruteForce(price, n - 1);
    for (int piece = 2; piece <= n; ++piece)
        best = std::max(best, price[piece - 1] + bruteForce(price, n - piece));
    return best;
}

}  // namespace

TEST(RodCutting, ClrsPriceTable) {
    RodCutting rod({1, 5, 8, 9, 10, 17, 17, 20, 24, 30});
    const std::vector<int> expected{0, 1, 5, 8, 10, 13, 17, 18, 22, 25, 30};
    for (int n = 0; n <= 10; ++n) EXPECT_EQ(rod.bestRevenue(n), expected[n]) << "n=" << n;
}

TEST(RodCutting, CutsSumToTheLengthAndEarnTheRevenue) {
    RodCutting rod({1, 5, 8, 9, 10, 17, 17, 20, 24, 30});
    for (int n = 0; n <= 10; ++n) {
        const auto pieces = rod.cuts(n);
        EXPECT_EQ(std::accumulate(pieces.begin(), pieces.end(), 0), n);
        int revenue = 0;
        for (int p : pieces) revenue += std::vector<int>{1, 5, 8, 9, 10, 17, 17, 20, 24, 30}[p - 1];
        EXPECT_EQ(revenue, rod.bestRevenue(n));
    }
}

TEST(RodCutting, RejectsLengthsWithoutAPrice) {
    RodCutting rod({2, 3});
    EXPECT_THROW(rod.bestRevenue(3), std::out_of_range);
    EXPECT_THROW(rod.cuts(-1), std::out_of_range);
}

TEST(RodCutting, NegativePricesStillTerminate) {
    RodCutting rod({-5, -1});
    EXPECT_EQ(rod.bestRevenue(2), -1);
    EXPECT_EQ(rod.cuts(2), (std::vector<int>{2}));
}

TEST(RodCutting, RandomizedMatchesExhaustiveSearch) {
    testutil::Rng rng(6);
    for (int iter = 0; iter < 300; ++iter) {
        const int n = testutil::randInt(rng, 0, 12);
        std::vector<int> price(static_cast<std::size_t>(std::max(n, 1)));
        for (auto& p : price) p = testutil::randInt(rng, 0, 20);
        RodCutting rod(price);
        EXPECT_EQ(rod.bestRevenue(n), bruteForce(price, n));
    }
}

TEST(RodCutting, LowestPriceHasAReconstructibleCut) {
    const int lo = std::numeric_limits<int>::min();
    RodCutting rod({lo, lo});
    EXPECT_EQ(rod.bestRevenue(1), lo);
    EXPECT_EQ(rod.cuts(1), (std::vector<int>{1}));
    // Two length-one pieces underflow int, but the uncut rod is representable and better.
    EXPECT_EQ(rod.bestRevenue(2), lo);
    EXPECT_EQ(rod.cuts(2), (std::vector<int>{2}));
}

TEST(RodCutting, UnrepresentableRevenueThrows) {
    RodCutting rod({std::numeric_limits<int>::max(), 0});
    EXPECT_THROW(rod.bestRevenue(2), std::overflow_error);
    EXPECT_THROW(rod.cuts(2), std::overflow_error);
}
