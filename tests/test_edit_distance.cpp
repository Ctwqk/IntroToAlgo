#include <gtest/gtest.h>

#include <algorithm>
#include <queue>
#include <set>
#include <string>

#include "introalgo/dynamic_programming/edit_distance.hpp"
#include "test_utils.hpp"

using introalgo::ED;

namespace {

// Exhaustive oracle: breadth-first search over all strings reachable by single insertions,
// deletions and substitutions (alphabet {a, b}, lengths capped at |a| + |b|).
int bfsEditDistance(const std::string& from, const std::string& to) {
    const std::size_t cap = from.size() + to.size();
    std::set<std::string> seen{from};
    std::queue<std::pair<std::string, int>> q;
    q.push({from, 0});
    while (!q.empty()) {
        const std::string s = q.front().first;
        const int d = q.front().second;
        q.pop();
        if (s == to) return d;
        auto visit = [&](const std::string& next) {
            if (seen.insert(next).second) q.push({next, d + 1});
        };
        for (std::size_t i = 0; i < s.size(); ++i) {
            visit(s.substr(0, i) + s.substr(i + 1));  // delete
            for (char c : {'a', 'b'}) {
                std::string t = s;
                t[i] = c;
                if (t != s) visit(t);  // substitute
            }
        }
        if (s.size() < cap)
            for (std::size_t i = 0; i <= s.size(); ++i)
                for (char c : {'a', 'b'}) visit(s.substr(0, i) + c + s.substr(i));  // insert
    }
    return -1;
}

}  // namespace

TEST(EditDistance, KnownAnswers) {
    EXPECT_EQ(ED("kitten", "sitting").calculate(), 3);
    EXPECT_EQ(ED("flaw", "lawn").calculate(), 2);
    EXPECT_EQ(ED("intention", "execution").calculate(), 5);
    EXPECT_EQ(ED("", "abc").calculate(), 3);
    EXPECT_EQ(ED("abc", "").calculate(), 3);
    EXPECT_EQ(ED("", "").calculate(), 0);
}

TEST(EditDistance, FirstArgumentIsUsed) {
    // The original constructor initialised both strings from the second argument, so every
    // distance was computed between a string and itself.
    EXPECT_EQ(ED("abc", "xyz").calculate(), 3);
    EXPECT_NE(ED("abc", "abd").calculate(), 0);
}

TEST(EditDistance, MatchingCharactersAreFree) {
    EXPECT_EQ(ED("algorithm", "algorithm").calculate(), 0);
    EXPECT_EQ(ED("abcdef", "abXdef").calculate(), 1);
}

TEST(EditDistance, RandomizedMatchesBreadthFirstSearch) {
    testutil::Rng rng(2);
    for (int iter = 0; iter < 200; ++iter) {
        auto a = testutil::randString(rng, std::size_t(testutil::randInt(rng, 0, 5)), "ab");
        auto b = testutil::randString(rng, std::size_t(testutil::randInt(rng, 0, 5)), "ab");
        EXPECT_EQ(ED(a, b).calculate(), bfsEditDistance(a, b)) << "a=" << a << " b=" << b;
    }
}

TEST(EditDistance, MetricProperties) {
    testutil::Rng rng(3);
    for (int iter = 0; iter < 200; ++iter) {
        auto a = testutil::randString(rng, std::size_t(testutil::randInt(rng, 0, 12)), "abcd");
        auto b = testutil::randString(rng, std::size_t(testutil::randInt(rng, 0, 12)), "abcd");
        auto c = testutil::randString(rng, std::size_t(testutil::randInt(rng, 0, 12)), "abcd");
        const int ab = ED(a, b).calculate(), ba = ED(b, a).calculate();
        EXPECT_EQ(ab, ba);
        EXPECT_EQ(ED(a, a).calculate(), 0);
        EXPECT_LE(ab, ED(a, c).calculate() + ED(c, b).calculate());  // triangle inequality
        EXPECT_GE(ab, std::abs(int(a.size()) - int(b.size())));
        EXPECT_LE(ab, int(std::max(a.size(), b.size())));
    }
}
