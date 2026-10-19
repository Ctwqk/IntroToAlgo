#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "introalgo/strings/rolling_hash.hpp"
#include "test_utils.hpp"

using introalgo::DNAMatch;

namespace {

std::vector<int> naive(const std::string& text, const std::string& pattern) {
    std::vector<int> out;
    if (pattern.empty()) return out;
    for (std::size_t i = 0; i + pattern.size() <= text.size(); ++i)
        if (text.compare(i, pattern.size(), pattern) == 0) out.push_back(static_cast<int>(i));
    return out;
}

}  // namespace

TEST(DnaMatch, KnownAnswerFromOriginalDemo) {
    const std::string target =
        "TATTACTAGCTACTGTACTCGGCAGCCTATTCACTAGCTACTGTACTGGAGTACTATTCGGTGCGTATTCGACTAGCTACTGTACTGATA"
        "ACGTACGT";
    DNAMatch d("ACTAGCTACTGTACT", target);
    EXPECT_EQ(d.match(), (std::vector<int>{4, 32, 71}));
}

TEST(DnaMatch, CallingMatchTwiceGivesTheSameAnswer) {
    // The original appended to a member vector, so every extra call duplicated the matches.
    DNAMatch d("ACG", "ACGTACG");
    EXPECT_EQ(d.match(), (std::vector<int>{0, 4}));
    EXPECT_EQ(d.match(), (std::vector<int>{0, 4}));
}

TEST(DnaMatch, LongPatterns) {
    // The original shifted an int by 2m-2 bits: patterns of more than 16 letters were broken.
    testutil::Rng rng(12);
    const std::string text = testutil::randString(rng, 500, "ACGT");
    for (std::size_t len : {17u, 20u, 31u, 33u, 64u, 200u}) {
        const std::string pattern = text.substr(5, len);
        DNAMatch d(pattern, text);
        EXPECT_EQ(d.match(), naive(text, pattern)) << "len=" << len;
        EXPECT_FALSE(d.match().empty());
    }
}

TEST(DnaMatch, EdgeCases) {
    EXPECT_TRUE(DNAMatch("", "ACGT").match().empty());
    EXPECT_TRUE(DNAMatch("ACGTA", "ACGT").match().empty());  // pattern longer than the text
    EXPECT_EQ(DNAMatch("ACGT", "ACGT").match(), (std::vector<int>{0}));
    EXPECT_EQ(DNAMatch("AA", "AAAA").match(), (std::vector<int>{0, 1, 2}));
}

TEST(DnaMatch, RejectsNonDnaInputAndBadModulus) {
    EXPECT_THROW(DNAMatch("ACX", "ACGT"), std::invalid_argument);
    EXPECT_THROW(DNAMatch("AC", "ACGU"), std::invalid_argument);
    EXPECT_THROW(DNAMatch("AC", "ACGT", 1), std::invalid_argument);
}

TEST(DnaMatch, RandomizedMatchesNaiveSearch) {
    testutil::Rng rng(13);
    for (int iter = 0; iter < 1000; ++iter) {
        auto text = testutil::randString(rng, std::size_t(testutil::randInt(rng, 0, 60)), "ACGT");
        auto pattern = testutil::randString(rng, std::size_t(testutil::randInt(rng, 0, 8)), "AC");
        EXPECT_EQ(DNAMatch(pattern, text).match(), naive(text, pattern));
    }
}

TEST(DnaMatch, HashCollisionsDoNotProduceFalseMatches) {
    // With a tiny modulus almost every window collides with the pattern's hash; the character
    // by character verification must still keep the answer exact.
    testutil::Rng rng(14);
    for (long long mod : {2LL, 3LL, 5LL, 7LL, 13LL}) {
        for (int iter = 0; iter < 300; ++iter) {
            auto text =
                testutil::randString(rng, std::size_t(testutil::randInt(rng, 0, 50)), "ACGT");
            auto pattern =
                testutil::randString(rng, std::size_t(testutil::randInt(rng, 1, 7)), "ACGT");
            EXPECT_EQ(DNAMatch(pattern, text, mod).match(), naive(text, pattern)) << "mod=" << mod;
        }
    }
}
