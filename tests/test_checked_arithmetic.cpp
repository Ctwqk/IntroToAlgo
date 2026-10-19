#include <gtest/gtest.h>

#include <limits>

#include "../src/checked_arithmetic.hpp"

using namespace introalgo::detail;

TEST(CheckedArithmetic, AdditionAtSignedLimits) {
    const auto lo = std::numeric_limits<long long>::min();
    const auto hi = std::numeric_limits<long long>::max();
    EXPECT_EQ(checkedAdd(hi, 0), hi);
    EXPECT_EQ(checkedAdd(lo, 0), lo);
    EXPECT_EQ(checkedAdd(hi, lo), -1);
    EXPECT_EQ(checkedAdd(lo + 1, -1), lo);
    EXPECT_THROW(checkedAdd(hi, 1), std::overflow_error);
    EXPECT_THROW(checkedAdd(lo, -1), std::overflow_error);
    EXPECT_THROW(checkedAdd(lo, lo), std::overflow_error);
}

TEST(CheckedArithmetic, SubtractionAtSignedLimits) {
    const auto lo = std::numeric_limits<long long>::min();
    const auto hi = std::numeric_limits<long long>::max();
    EXPECT_EQ(checkedSubtract(lo, lo), 0);
    EXPECT_EQ(checkedSubtract(hi, hi), 0);
    EXPECT_EQ(checkedSubtract(-1, lo), hi);
    EXPECT_EQ(checkedSubtract(lo + 1, 1), lo);
    EXPECT_THROW(checkedSubtract(lo, 1), std::overflow_error);
    EXPECT_THROW(checkedSubtract(hi, -1), std::overflow_error);
    EXPECT_THROW(checkedSubtract(hi, lo), std::overflow_error);
}

TEST(CheckedArithmetic, MultiplicationForEverySignAndZero) {
    const auto lo = std::numeric_limits<long long>::min();
    const auto hi = std::numeric_limits<long long>::max();
    EXPECT_EQ(checkedMultiply(lo, 0), 0);
    EXPECT_EQ(checkedMultiply(0, lo), 0);
    EXPECT_EQ(checkedMultiply(lo, 1), lo);
    EXPECT_EQ(checkedMultiply(hi, -1), -hi);
    EXPECT_EQ(checkedMultiply(-7, -6), 42);
    EXPECT_EQ(checkedMultiply(7, -6), -42);
    EXPECT_EQ(checkedMultiply(-7, 6), -42);
    EXPECT_EQ(checkedMultiply(lo / 2, 2), lo);
    EXPECT_THROW(checkedMultiply(lo, -1), std::overflow_error);
    EXPECT_THROW(checkedMultiply(-1, lo), std::overflow_error);
    EXPECT_THROW(checkedMultiply(hi, 2), std::overflow_error);
    EXPECT_THROW(checkedMultiply(2, lo), std::overflow_error);
    EXPECT_THROW(checkedMultiply(lo, 2), std::overflow_error);
    EXPECT_THROW(checkedMultiply(-3037000500LL, -3037000500LL), std::overflow_error);
}
