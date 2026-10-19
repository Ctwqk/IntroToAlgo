#include <gtest/gtest.h>

#include <algorithm>
#include <numeric>

#include "introalgo/divide_and_conquer/out_of_ranking.hpp"
#include "test_utils.hpp"

using introalgo::OutOfRanking;

namespace {

long long bruteForceInversions(const std::vector<int>& v) {
    long long inv = 0;
    for (std::size_t i = 0; i < v.size(); ++i)
        for (std::size_t j = i + 1; j < v.size(); ++j)
            if (v[i] > v[j]) ++inv;
    return inv;
}

}  // namespace

TEST(OutOfRanking, KnownAnswerFromOriginalDemo) {
    std::vector<int> rank{2, 4, 5, 1, 3, 6, 7};
    OutOfRanking o;
    EXPECT_EQ(o.user_similarity(rank), 5);
    EXPECT_TRUE(std::is_sorted(rank.begin(), rank.end()));
}

TEST(OutOfRanking, EmptyAndSingleElement) {
    OutOfRanking o;
    std::vector<int> empty;
    EXPECT_EQ(o.user_similarity(empty), 0);
    std::vector<int> one{42};
    EXPECT_EQ(o.user_similarity(one), 0);
}

TEST(OutOfRanking, SortedHasNoInversionsReversedHasAll) {
    OutOfRanking o;
    std::vector<int> sorted(100);
    std::iota(sorted.begin(), sorted.end(), 0);
    EXPECT_EQ(o.user_similarity(sorted), 0);
    std::vector<int> reversed(sorted.rbegin(), sorted.rend());
    EXPECT_EQ(o.user_similarity(reversed), 100 * 99 / 2);
}

TEST(OutOfRanking, EqualElementsAreNotInversions) {
    OutOfRanking o;
    std::vector<int> v{3, 3, 3, 3};
    EXPECT_EQ(o.user_similarity(v), 0);
    std::vector<int> w{2, 1, 2, 1};  // (2,1) (2,1) (2,1): positions (0,1) (0,3) (2,3)
    EXPECT_EQ(o.user_similarity(w), 3);
}

TEST(OutOfRanking, ObjectCanBeReused) {
    // The original kept a running counter in the object and never reset it.
    OutOfRanking o;
    std::vector<int> a{2, 1};
    std::vector<int> b{2, 1};
    EXPECT_EQ(o.user_similarity(a), 1);
    EXPECT_EQ(o.user_similarity(b), 1);
}

TEST(OutOfRanking, CountDoesNotOverflow32Bits) {
    // n = 70000 reversed has n(n-1)/2 = 2'449'965'000 inversions > INT_MAX.
    const int n = 70000;
    std::vector<int> v(n);
    for (int i = 0; i < n; ++i) v[i] = n - i;
    OutOfRanking o;
    EXPECT_EQ(o.user_similarity(v), 2449965000LL);
}

TEST(OutOfRanking, RandomizedMatchesQuadraticOracle) {
    testutil::Rng rng(20250101);
    OutOfRanking o;
    for (int iter = 0; iter < 500; ++iter) {
        auto v = testutil::randVector(rng, std::size_t(testutil::randInt(rng, 0, 60)), -20, 20);
        auto expected = bruteForceInversions(v);
        auto sorted = v;
        std::sort(sorted.begin(), sorted.end());
        EXPECT_EQ(o.user_similarity(v), expected);
        EXPECT_EQ(v, sorted) << "input must be left sorted";
    }
}
