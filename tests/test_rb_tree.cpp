#include <gtest/gtest.h>

#include <algorithm>
#include <cmath>
#include <set>
#include <string>
#include <vector>

#include "introalgo/data_structures/rb_tree.hpp"
#include "test_utils.hpp"

using introalgo::RbTree;

namespace {

void expectValid(const RbTree& t) {
    std::string why;
    EXPECT_TRUE(t.validate(&why)) << why;
}

}  // namespace

TEST(RbTree, EmptyTree) {
    RbTree t;
    expectValid(t);
    EXPECT_EQ(t.size(), 0u);
    EXPECT_EQ(t.height(), 0);
    EXPECT_FALSE(t.contains(1));
    EXPECT_FALSE(t.erase(1));
}

TEST(RbTree, InsertKeepsTheTreeOrderedAndValid) {
    RbTree t;
    for (int v : {10, 33, 44, 1323, 42, 99, 0, 1, 4, 29, 4, 5}) {
        t.insert(v);
        expectValid(t);
    }
    EXPECT_EQ(t.inorder(), (std::vector<int>{0, 1, 4, 4, 5, 10, 29, 33, 42, 44, 99, 1323}));
    EXPECT_TRUE(t.contains(1323));
    EXPECT_FALSE(t.contains(2));
}

TEST(RbTree, SortedInsertionStaysLogarithmic) {
    // A plain BST degenerates into a path for sorted input; the red-black rules cap the height
    // at 2 log2(n + 1).
    RbTree t;
    const int n = 100000;
    for (int i = 0; i < n; ++i) t.insert(i);
    expectValid(t);
    EXPECT_LE(t.height(), int(2 * std::log2(n + 1.0)));
    for (int i = 0; i < n; i += 2) EXPECT_TRUE(t.erase(i));
    expectValid(t);
    EXPECT_EQ(t.size(), std::size_t(n / 2));
    EXPECT_LE(t.height(), int(2 * std::log2(n / 2 + 1.0)));
}

TEST(RbTree, DuplicatesAreErasedOneAtATime) {
    RbTree t;
    for (int i = 0; i < 10; ++i) t.insert(5);
    for (int i = 0; i < 10; ++i) {
        EXPECT_TRUE(t.contains(5));
        EXPECT_TRUE(t.erase(5));
        expectValid(t);
    }
    EXPECT_FALSE(t.contains(5));
}

TEST(RbTree, TreesDoNotShareState) {
    // The original used one static Nil sentinel for every tree.
    RbTree a, b;
    for (int i = 0; i < 100; ++i) a.insert(i);
    for (int i = 100; i < 150; ++i) b.insert(i);
    for (int i = 0; i < 100; i += 3) a.erase(i);
    expectValid(a);
    expectValid(b);
    EXPECT_FALSE(a.contains(120));
    EXPECT_TRUE(b.contains(120));
}

TEST(RbTree, RandomizedOperationsMatchMultisetAndKeepAllInvariants) {
    for (unsigned seed = 1; seed <= 6; ++seed) {
        testutil::Rng rng(100 + seed);
        RbTree t;
        std::multiset<int> oracle;
        const int range = seed % 2 ? 30 : 500;  // small ranges force many duplicates
        for (int step = 0; step < 4000; ++step) {
            const int v = testutil::randInt(rng, 0, range);
            if (testutil::randInt(rng, 0, 2) != 0) {
                t.insert(v);
                oracle.insert(v);
            } else {
                auto it = oracle.find(v);
                ASSERT_EQ(t.erase(v), it != oracle.end());
                if (it != oracle.end()) oracle.erase(it);
            }
            ASSERT_EQ(t.size(), oracle.size());
            if (step % 20 == 0) {
                std::string why;
                ASSERT_TRUE(t.validate(&why)) << why << " (seed " << seed << ", step " << step << ")";
                ASSERT_EQ(t.inorder(), std::vector<int>(oracle.begin(), oracle.end()));
            }
        }
        expectValid(t);
    }
}
