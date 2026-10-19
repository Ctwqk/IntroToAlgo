#include <gtest/gtest.h>

#include <algorithm>
#include <stdexcept>
#include <utility>
#include <vector>

#include "introalgo/dynamic_programming/knapsack_01.hpp"
#include "test_utils.hpp"

using introalgo::Knapsack;
using Items = std::vector<std::pair<int, int>>;  // (value, weight)

namespace {

int bruteForce(const Items& items, int capacity) {
    int best = 0;
    for (unsigned mask = 0; mask < (1u << items.size()); ++mask) {
        int value = 0, weight = 0;
        for (std::size_t i = 0; i < items.size(); ++i)
            if (mask & (1u << i)) {
                value += items[i].first;
                weight += items[i].second;
            }
        if (weight <= capacity) best = std::max(best, value);
    }
    return best;
}

}  // namespace

TEST(Knapsack, KnownAnswerFromOriginalDemo) {
    Knapsack k({{100, 20}, {120, 30}, {60, 10}}, 50);
    EXPECT_EQ(k.solve(), 220);
    EXPECT_EQ(k.chosenItems(), (std::vector<int>{0, 1}));
}

TEST(Knapsack, ChosenItemsFitAndAreWorthTheOptimum) {
    Items items{{100, 20}, {120, 30}, {60, 10}};
    Knapsack k(items, 50);
    const int best = k.solve();
    int value = 0, weight = 0;
    for (int i : k.chosenItems()) {
        value += items[i].first;
        weight += items[i].second;
    }
    EXPECT_EQ(value, best);
    EXPECT_LE(weight, 50);
}

TEST(Knapsack, ItemThatDoesNotFitDoesNotErasePreviousRow) {
    // In the original an item heavier than the current capacity left a 0 in the table, and the
    // answer was patched afterwards with a max over rows.  Here that patch returns 20 instead
    // of 30.
    Knapsack k({{10, 1}, {100, 50}, {20, 1}}, 3);
    EXPECT_EQ(k.solve(), 30);
}

TEST(Knapsack, DegenerateInputs) {
    EXPECT_EQ(Knapsack({}, 10).solve(), 0);
    EXPECT_EQ(Knapsack({{5, 1}}, 0).solve(), 0);
    EXPECT_EQ(Knapsack({{5, 0}}, 0).solve(), 5);   // weightless items are always taken
    EXPECT_EQ(Knapsack({{5, 1}}, -3).solve(), 0);  // negative capacity behaves like zero
    EXPECT_THROW(Knapsack({{5, -1}}, 3), std::invalid_argument);
}

TEST(Knapsack, RandomizedMatchesExhaustiveSearch) {
    testutil::Rng rng(5);
    for (int iter = 0; iter < 500; ++iter) {
        Items items;
        const int n = testutil::randInt(rng, 0, 12);
        for (int i = 0; i < n; ++i)
            items.push_back({testutil::randInt(rng, 0, 30), testutil::randInt(rng, 0, 12)});
        const int capacity = testutil::randInt(rng, 0, 40);
        Knapsack k(items, capacity);
        EXPECT_EQ(k.solve(), bruteForce(items, capacity));
        int value = 0, weight = 0;
        for (int i : k.chosenItems()) {
            value += items[i].first;
            weight += items[i].second;
        }
        EXPECT_EQ(value, bruteForce(items, capacity));
        EXPECT_LE(weight, capacity);
    }
}
