#include <gtest/gtest.h>

#include <algorithm>
#include <cmath>
#include <numeric>
#include <vector>

#include "introalgo/math/optimal_stopping.hpp"

using introalgo::CalFormula;

namespace {

// Exact success probability of "reject the first x, then hire the first candidate who beats
// everybody seen so far", by trying all n! arrival orders (ranks 1 = worst .. n = best).
double exhaustiveProbability(int n, int x) {
    std::vector<int> rank(static_cast<std::size_t>(n));
    std::iota(rank.begin(), rank.end(), 1);
    long long wins = 0, total = 0;
    do {
        ++total;
        int bestSeen = 0;
        for (int i = 0; i < x; ++i) bestSeen = std::max(bestSeen, rank[std::size_t(i)]);
        for (int i = x; i < n; ++i) {
            if (rank[std::size_t(i)] > bestSeen) {
                if (rank[std::size_t(i)] == n) ++wins;
                break;
            }
        }
    } while (std::next_permutation(rank.begin(), rank.end()));
    return double(wins) / double(total);
}

}  // namespace

TEST(OptimalStopping, OriginalDemoRejectsSeventyThreeOfTwoHundred) {
    CalFormula c(200);
    EXPECT_EQ(c.getMax(), 73);
    EXPECT_NEAR(c.bestProbability(), 0.369461, 1e-6);
    EXPECT_NEAR(c.getMax() / 200.0, 1 / std::exp(1.0), 0.01);  // about n / e
}

TEST(OptimalStopping, FormulaMatchesExhaustiveEnumeration) {
    for (int n = 1; n <= 8; ++n) {
        CalFormula c(n);
        for (int x = 1; x <= n; ++x)
            EXPECT_NEAR(c.calculate(x), exhaustiveProbability(n, x), 1e-12) << "n=" << n << " x=" << x;
    }
}

TEST(OptimalStopping, GetMaxPicksTheBestRejectionCountAmongAllChoices) {
    for (int n = 2; n <= 8; ++n) {
        CalFormula c(n);
        int bestX = 1;
        for (int x = 1; x <= n; ++x)
            if (exhaustiveProbability(n, x) > exhaustiveProbability(n, bestX) + 1e-12) bestX = x;
        EXPECT_EQ(c.getMax(), bestX) << "n=" << n;
        EXPECT_NEAR(c.bestProbability(), exhaustiveProbability(n, bestX), 1e-12);
    }
}

TEST(OptimalStopping, ProbabilityTendsToOneOverE) {
    CalFormula c(20000);
    c.getMax();
    EXPECT_NEAR(c.bestProbability(), 1 / std::exp(1.0), 1e-3);
}

TEST(OptimalStopping, EdgeCases) {
    CalFormula one(1);
    EXPECT_EQ(one.getMax(), 0);  // the formula only covers x >= 1
    EXPECT_EQ(one.calculate(0), 0.0);
    CalFormula three(3);
    EXPECT_EQ(three.calculate(-1), 0.0);
    EXPECT_EQ(three.calculate(4), 0.0);
    EXPECT_EQ(three.calculate(3), 0.0);  // rejecting everybody never hires anybody
    EXPECT_THROW(CalFormula(0), std::invalid_argument);
}
