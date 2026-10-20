#include <gtest/gtest.h>

#include <algorithm>
#include <climits>
#include <functional>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

#include "introalgo/sorting/sort.hpp"
#include "test_utils.hpp"

using introalgo::DigitKey;
using introalgo::Sort;

namespace {

using Algo = std::function<void(Sort&)>;

const std::vector<std::pair<const char*, Algo>>& algorithms() {
    static const std::vector<std::pair<const char*, Algo>> all{
        {"bubble", [](Sort& s) { s.bubble_sort(); }},
        {"insertion", [](Sort& s) { s.insert_sort(); }},
        {"selection", [](Sort& s) { s.select_sort(); }},
        {"quick/hole", [](Sort& s) { s.quick_sort(Sort::Partition::Hole); }},
        {"quick/lomuto", [](Sort& s) { s.quick_sort(Sort::Partition::Lomuto); }},
        {"counting", [](Sort& s) { s.counting_sort(); }},
    };
    return all;
}

void expectSortsLikeStd(const std::vector<int>& input, bool includeCounting = true) {
    auto expected = input;
    std::sort(expected.begin(), expected.end());
    for (const auto& [name, run] : algorithms()) {
        if (!includeCounting && std::string(name) == "counting") continue;
        Sort s(input);
        run(s);
        EXPECT_EQ(s.data(), expected) << name << " on input of size " << input.size();
    }
}

}  // namespace

TEST(Sort, KnownInput) {
    // The commented-out main() of the original.
    expectSortsLikeStd({3, 5, 1, 3, 5, 7, 8, 3, 2, 5, 10, 33, 44, 1323, 42, 99, 0, 1, 4, 29, 4, 5});
}

TEST(Sort, EdgeCases) {
    expectSortsLikeStd({});
    expectSortsLikeStd({1});
    expectSortsLikeStd({2, 1});
    expectSortsLikeStd({7, 7, 7, 7});
    expectSortsLikeStd({INT_MAX, INT_MIN, 0, -1, 1},
                       /*includeCounting=*/false);  // range too wide for it
}

TEST(Sort, SortedReversedAndConstantInputs) {
    std::vector<int> ascending(500);
    std::iota(ascending.begin(), ascending.end(), -250);
    expectSortsLikeStd(ascending);
    expectSortsLikeStd(std::vector<int>(ascending.rbegin(), ascending.rend()));
    expectSortsLikeStd(std::vector<int>(300, 4));
}

TEST(Sort, RandomizedAgainstStdSort) {
    testutil::Rng rng(41);
    for (int iter = 0; iter < 300; ++iter) {
        const int range = testutil::randInt(rng, 0, 1) ? 5 : 1000;  // many duplicates / few
        auto input =
            testutil::randVector(rng, std::size_t(testutil::randInt(rng, 0, 80)), -range, range);
        auto expected = input;
        std::sort(expected.begin(), expected.end());
        for (const auto& [name, run] : algorithms()) {
            Sort s(input);
            run(s);
            ASSERT_EQ(s.data(), expected) << name;
        }
    }
}

TEST(Sort, QuickSortSortsASubrangeOnly) {
    Sort s({9, 8, 7, 6, 5, 4, 3});
    s.quick_sort(1, 4);
    EXPECT_EQ(s.data(), (std::vector<int>{9, 5, 6, 7, 8, 4, 3}));
    EXPECT_THROW(s.quick_sort(0, 7), std::out_of_range);
    EXPECT_THROW(s.quick_sort(-1, 2), std::out_of_range);
}

TEST(Sort, PartitionSchemesPlaceThePivot) {
    for (auto part : {&Sort::partition_qsa_1, &Sort::partition_qsa_2}) {
        testutil::Rng rng(42);
        for (int iter = 0; iter < 200; ++iter) {
            auto v = testutil::randVector(rng, std::size_t(testutil::randInt(rng, 2, 30)), 0, 9);
            Sort s(v);
            const int hi = int(v.size()) - 1;
            const int p = (s.*part)(0, hi);
            ASSERT_GE(p, 0);
            ASSERT_LE(p, hi);
            const auto& d = s.data();
            for (int i = 0; i < p; ++i) EXPECT_LE(d[i], d[p]);
            for (int i = p + 1; i <= hi; ++i) EXPECT_GE(d[i], d[p]);
            auto a = v, b = d;  // a permutation of the input
            std::sort(a.begin(), a.end());
            std::sort(b.begin(), b.end());
            EXPECT_EQ(a, b);
        }
    }
    Sort s({1});
    EXPECT_EQ(s.partition_qsa_1(0, 0), -1);
    EXPECT_EQ(s.partition_qsa_2(0, 0), -1);
}

TEST(Sort, QuickSortHandlesLargeSortedInputWithoutDeepRecursion) {
    // Sorted input is the worst case for a first/last-element pivot: the recursion of a naive
    // quick sort goes n levels deep.  Here the smaller side is always handled first.
    std::vector<int> v(20000);
    std::iota(v.begin(), v.end(), 0);
    Sort s(v);
    s.quick_sort(Sort::Partition::Hole);
    EXPECT_EQ(s.data(), v);
}

TEST(Sort, CountingSortRejectsHugeRanges) {
    // INT_MAX - INT_MIN + 1 overflowed int in the original and allocated garbage.
    Sort s({INT_MIN, INT_MAX});
    EXPECT_THROW(s.counting_sort(), std::length_error);
    Sort ok({-5, 3, -5, 0});
    ok.counting_sort();
    EXPECT_EQ(ok.data(), (std::vector<int>{-5, -5, 0, 3}));
}

TEST(Sort, CountingSortIsStable) {
    // Sort records by key with the radix helper (one digit) and look at the satellite data.
    std::vector<DigitKey> recs;
    const int keys[] = {2, 1, 2, 1, 0, 2};
    for (int i = 0; i < 6; ++i) {
        DigitKey d(std::vector<int>{keys[i]});
        d.data = i;
        recs.push_back(d);
    }
    Sort().radix_sort(recs, 1);
    std::vector<int> order;
    for (const auto& r : recs) order.push_back(r.data);
    EXPECT_EQ(order, (std::vector<int>{4, 1, 3, 0, 2, 5}));
}

TEST(Sort, RadixSortOrdersRecordsLexicographically) {
    testutil::Rng rng(43);
    for (int iter = 0; iter < 100; ++iter) {
        const int digits = testutil::randInt(rng, 1, 5);
        const int n = testutil::randInt(rng, 0, 60);
        std::vector<DigitKey> recs;
        for (int i = 0; i < n; ++i) {
            DigitKey d(testutil::randVector(rng, std::size_t(digits), -3, 6));
            d.data = i;
            recs.push_back(d);
        }
        auto expected = recs;
        std::stable_sort(expected.begin(), expected.end(),
                         [](const DigitKey& a, const DigitKey& b) { return a.key < b.key; });
        Sort().radix_sort(recs, digits);
        ASSERT_EQ(recs.size(), expected.size());
        for (std::size_t i = 0; i < recs.size(); ++i) {
            EXPECT_EQ(recs[i].key, expected[i].key);
            EXPECT_EQ(recs[i].data, expected[i].data) << "radix sort must be stable";
        }
    }
}

TEST(Sort, RadixSortRejectsShortRecords) {
    std::vector<DigitKey> recs{DigitKey(std::vector<int>{1, 2}), DigitKey(std::vector<int>{1})};
    EXPECT_THROW(Sort().radix_sort(recs, 2), std::invalid_argument);
}

TEST(Sort, RadixSortRejectsNegativeDigitCounts) {
    std::vector<DigitKey> recs{DigitKey(std::vector<int>{2}), DigitKey(std::vector<int>{1})};
    for (int digits : {-1, INT_MIN})
        EXPECT_THROW(Sort().radix_sort(recs, digits), std::invalid_argument);
    std::vector<DigitKey> empty;
    EXPECT_THROW(Sort().radix_sort(empty, -1), std::invalid_argument);
    Sort().radix_sort(recs, 0);
    EXPECT_EQ(recs[0].key, (std::vector<int>{2}));
    EXPECT_EQ(recs[1].key, (std::vector<int>{1}));
}

TEST(Sort, CheckoutAndDigitKeyHelpers) {
    Sort s({3, 1, 2});
    s.insert_sort();
    std::ostringstream out;
    s.checkout(out);
    EXPECT_EQ(out.str(), "1 2 3 ");

    DigitKey a(std::vector<int>{1, 9}), b(std::vector<int>{1, 3});
    EXPECT_FALSE(a.comp(b, 0));
    EXPECT_TRUE(a.comp(b, 1));
    std::ostringstream shown;
    a.show(shown);
    EXPECT_EQ(shown.str(), "1 9 \n");
    EXPECT_EQ(DigitKey(3).digits(), 3);
}
