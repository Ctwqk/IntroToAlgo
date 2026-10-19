#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "introalgo/strings/kmp.hpp"
#include "test_utils.hpp"

using introalgo::KMP;

namespace {

// Oracle: try every start position.
std::vector<int> naive(const std::string& text, const std::string& pattern) {
    std::vector<int> out;
    if (pattern.empty()) return out;
    for (std::size_t i = 0; i + pattern.size() <= text.size(); ++i)
        if (text.compare(i, pattern.size(), pattern) == 0) out.push_back(static_cast<int>(i));
    return out;
}

// Oracle for the prefix function: longest proper border of every prefix, by definition.
std::vector<int> naivePrefixFunction(const std::string& p) {
    std::vector<int> out(p.size(), 0);
    for (std::size_t i = 0; i < p.size(); ++i)
        for (std::size_t len = i; len >= 1; --len)
            if (p.compare(0, len, p, i + 1 - len, len) == 0) {
                out[i] = static_cast<int>(len);
                break;
            }
    return out;
}

}  // namespace

TEST(Kmp, KnownAnswerFromOriginalDemo) {
    const std::string target =
        "TATTACTAGCTACTGTACTCGGCAGCCTATTCACTAGCTACTGTACTGGAGTACTATTCGGTGCGTATTCGACTAGCTACTGTACTGATA"
        "ACGTACGT";
    KMP k(target, "ACTAGCTACTGTACT");
    EXPECT_EQ(k.calculateKMP(), (std::vector<int>{4, 32, 71}));
}

TEST(Kmp, OverlappingMatchesAreAllReported) {
    EXPECT_EQ(KMP("aaaa", "aa").calculateKMP(), (std::vector<int>{0, 1, 2}));
    EXPECT_EQ(KMP("abababa", "aba").calculateKMP(), (std::vector<int>{0, 2, 4}));
}

TEST(Kmp, EdgeCases) {
    EXPECT_TRUE(KMP("abc", "").calculateKMP().empty());
    EXPECT_TRUE(KMP("", "abc").calculateKMP().empty());
    EXPECT_TRUE(KMP("ab", "abc").calculateKMP().empty());
    EXPECT_EQ(KMP("abc", "abc").calculateKMP(), (std::vector<int>{0}));
    EXPECT_EQ(KMP("xabc", "c").calculateKMP(), (std::vector<int>{3}));
}

TEST(Kmp, PrefixFunctionFallsBackThroughShorterBorders) {
    // In the original the failure link after a fall-back was never extended by the matching
    // character, so patterns like this one got a wrong table and missed matches.
    EXPECT_EQ(KMP("bbabbabbb", "babb").calculateKMP(), (std::vector<int>{1, 4}));
    EXPECT_EQ(KMP("", "aabaaab").lps(), (std::vector<int>{0, 1, 0, 1, 2, 2, 3}));
}

TEST(Kmp, RandomizedMatchesNaiveSearch) {
    testutil::Rng rng(11);
    for (int iter = 0; iter < 2000; ++iter) {
        auto text = testutil::randString(rng, std::size_t(testutil::randInt(rng, 0, 30)), "ab");
        auto pattern = testutil::randString(rng, std::size_t(testutil::randInt(rng, 0, 6)), "ab");
        KMP k(text, pattern);
        EXPECT_EQ(k.calculateKMP(), naive(text, pattern))
            << "text=" << text << " pattern=" << pattern;
        EXPECT_EQ(k.lps(), naivePrefixFunction(pattern)) << "pattern=" << pattern;
    }
}

TEST(Kmp, LargeInputStaysLinear) {
    // 200k characters of "aaaa...": the quadratic naive search would need ~10^10 comparisons.
    std::string text(200000, 'a');
    std::string pattern(1000, 'a');
    pattern.back() = 'b';
    EXPECT_TRUE(KMP(text, pattern).calculateKMP().empty());
    EXPECT_EQ(KMP(text, std::string(1000, 'a')).calculateKMP().size(), 200000u - 1000u + 1u);
}
