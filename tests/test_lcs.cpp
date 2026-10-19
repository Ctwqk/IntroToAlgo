#include <gtest/gtest.h>

#include <algorithm>
#include <string>
#include <vector>

#include "introalgo/dynamic_programming/lcs.hpp"
#include "test_utils.hpp"

using introalgo::Lcs;

namespace {

bool isSubsequence(const std::string& sub, const std::string& s) {
    std::size_t k = 0;
    for (char c : s)
        if (k < sub.size() && sub[k] == c) ++k;
    return k == sub.size();
}

// Exhaustive oracle: try every subsequence of a (2^|a| of them) and keep the longest one that is
// also a subsequence of b.
int bruteForceLcsLength(const std::string& a, const std::string& b) {
    int best = 0;
    const std::size_t n = a.size();
    for (unsigned mask = 0; mask < (1u << n); ++mask) {
        std::string sub;
        for (std::size_t i = 0; i < n; ++i)
            if (mask & (1u << i)) sub += a[i];
        if (static_cast<int>(sub.size()) > best && isSubsequence(sub, b))
            best = static_cast<int>(sub.size());
    }
    return best;
}

void expectConsistent(const std::string& a, const std::string& b) {
    Lcs lcs(a, b);
    const int expected = bruteForceLcsLength(a, b);
    EXPECT_EQ(lcs.getLCSLength(), expected) << "a=" << a << " b=" << b;
    const std::string s = lcs.getLCS();
    EXPECT_EQ(static_cast<int>(s.size()), expected) << "a=" << a << " b=" << b;
    EXPECT_TRUE(isSubsequence(s, a)) << s << " is not a subsequence of " << a;
    EXPECT_TRUE(isSubsequence(s, b)) << s << " is not a subsequence of " << b;
}

}  // namespace

TEST(Lcs, KnownAnswerFromOriginalDemo) {
    Lcs lcs("10010101", "010110110");
    EXPECT_EQ(lcs.getLCSLength(), 6);
    EXPECT_EQ(lcs.getLCS().size(), 6u);
}

TEST(Lcs, ClassicTextbookPair) {
    // CLRS figure 15.8.
    Lcs lcs("ABCBDAB", "BDCABA");
    EXPECT_EQ(lcs.getLCSLength(), 4);
    EXPECT_EQ(lcs.getLCS().size(), 4u);
}

TEST(Lcs, EmptyStrings) {
    expectConsistent("", "");
    expectConsistent("abc", "");
    expectConsistent("", "abc");
}

TEST(Lcs, IdenticalAndDisjoint) {
    Lcs same("algorithm", "algorithm");
    EXPECT_EQ(same.getLCS(), "algorithm");
    Lcs disjoint("aaa", "bbb");
    EXPECT_EQ(disjoint.getLCSLength(), 0);
    EXPECT_EQ(disjoint.getLCS(), "");
}

TEST(Lcs, LengthIsSymmetric) {
    EXPECT_EQ(Lcs("AGGTAB", "GXTXAYB").getLCSLength(), Lcs("GXTXAYB", "AGGTAB").getLCSLength());
    EXPECT_EQ(Lcs("AGGTAB", "GXTXAYB").getLCS(), "GTAB");
}

TEST(Lcs, ExhaustiveOverBinaryStringsUpToLengthFour) {
    std::vector<std::string> all{""};
    for (std::size_t len = 1; len <= 4; ++len)
        for (unsigned m = 0; m < (1u << len); ++m) {
            std::string s;
            for (std::size_t i = 0; i < len; ++i) s += (m >> i & 1u) ? 'b' : 'a';
            all.push_back(s);
        }
    for (const auto& a : all)
        for (const auto& b : all) expectConsistent(a, b);
}

TEST(Lcs, RandomizedMatchesExhaustiveSearch) {
    testutil::Rng rng(1);
    for (int iter = 0; iter < 300; ++iter) {
        auto a = testutil::randString(rng, std::size_t(testutil::randInt(rng, 0, 11)), "abc");
        auto b = testutil::randString(rng, std::size_t(testutil::randInt(rng, 0, 14)), "abc");
        expectConsistent(a, b);
    }
}
