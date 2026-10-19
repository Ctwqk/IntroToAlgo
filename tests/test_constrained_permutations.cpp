#include <gtest/gtest.h>

#include <algorithm>
#include <numeric>
#include <sstream>
#include <vector>

#include "introalgo/backtracking/constrained_permutations.hpp"

using introalgo::ConstrainedPermutations;
using Perm = std::vector<int>;

namespace {

int longest(const Perm& p, bool increasing) {
    std::vector<int> best(p.size(), 1);
    int result = 0;
    for (std::size_t i = 0; i < p.size(); ++i) {
        for (std::size_t j = 0; j < i; ++j)
            if (increasing ? p[j] < p[i] : p[j] > p[i]) best[i] = std::max(best[i], best[j] + 1);
        result = std::max(result, best[i]);
    }
    return result;
}

// Exhaustive oracle: generate every permutation of 1..n and filter.
std::vector<Perm> bruteForce(int n, int maxInc, int maxDec) {
    Perm p(static_cast<std::size_t>(n));
    std::iota(p.begin(), p.end(), 1);
    std::vector<Perm> out;
    do {
        if (longest(p, true) <= maxInc && longest(p, false) <= maxDec) out.push_back(p);
    } while (std::next_permutation(p.begin(), p.end()));
    return out;
}

}  // namespace

TEST(ConstrainedPermutations, OriginalDemoHas1764Answers) {
    // 9 numbers, at most 3 increasing and at most 3 decreasing: 42^2 = 1764 (Erdos-Szekeres / RSK).
    ConstrainedPermutations c(9, 3, 3);
    c.find();
    EXPECT_EQ(c.answers().size(), 1764u);
    EXPECT_EQ(c.answers().front(), (Perm{3, 2, 1, 6, 5, 4, 9, 8, 7}));
    for (const auto& p : c.answers()) {
        ASSERT_LE(longest(p, true), 3);
        ASSERT_LE(longest(p, false), 3);
    }
    std::ostringstream out;
    c.show(out);
    EXPECT_NE(out.str().find("question: 9 number 3 increasing and 3 decreasing;"),
              std::string::npos);
    EXPECT_NE(out.str().find("number of answers: 1764"), std::string::npos);
}

TEST(ConstrainedPermutations, MatchesExhaustiveEnumerationForAllSmallThresholds) {
    for (int n = 0; n <= 7; ++n) {
        for (int inc = 0; inc <= n + 1; ++inc) {
            for (int dec = 0; dec <= n + 1; ++dec) {
                ConstrainedPermutations c(n, inc, dec);
                c.find();
                auto got = c.answers();
                auto expected = bruteForce(n, inc, dec);
                std::sort(got.begin(), got.end());
                std::sort(expected.begin(), expected.end());
                ASSERT_EQ(got, expected) << "n=" << n << " inc<=" << inc << " dec<=" << dec;
            }
        }
    }
}

TEST(ConstrainedPermutations, ThresholdsBelowOneAdmitNothing) {
    // Every non-empty sequence has an increasing and a decreasing subsequence of length 1.  The
    // original accepted the first position unconditionally and returned the one decreasing
    // permutation for (4, 0, 5) instead of none.
    ConstrainedPermutations c(4, 0, 5);
    c.find();
    EXPECT_TRUE(c.answers().empty());
    ConstrainedPermutations d(1, 1, 1);
    d.find();
    EXPECT_EQ(d.answers().size(), 1u);
    ConstrainedPermutations empty(0, 1, 1);
    empty.find();
    EXPECT_EQ(empty.answers().size(), 1u);  // the empty permutation
}

TEST(ConstrainedPermutations, FindCanBeRepeatedAndInputIsValidated) {
    ConstrainedPermutations c(5, 3, 3);
    c.find();
    const auto first = c.answers();
    c.find();  // the original carried its `layer` counter over between calls
    EXPECT_EQ(c.answers(), first);
    EXPECT_THROW(ConstrainedPermutations(-1, 1, 1), std::invalid_argument);
    EXPECT_THROW(ConstrainedPermutations(13, 1, 1), std::invalid_argument);
}

TEST(ConstrainedPermutations, UnpromisingPruningIsBounded) {
    ConstrainedPermutations c(12, 12, 12);  // no pruning at all: 12! partial permutations
    EXPECT_THROW(c.find(), std::length_error);
}
