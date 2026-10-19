#include <gtest/gtest.h>

#include <iterator>
#include <set>
#include <utility>

#include "introalgo/data_structures/van_emde_boas.hpp"
#include "test_utils.hpp"

using introalgo::VanEmdeBoas;

namespace {

int oracleSuccessor(const std::set<int>& s, int x) {
    auto it = s.upper_bound(x);
    return it == s.end() ? -1 : *it;
}

int oraclePredecessor(const std::set<int>& s, int x) {
    auto it = s.lower_bound(x);
    return it == s.begin() ? -1 : *std::prev(it);
}

void expectSameAsSet(const VanEmdeBoas& v, const std::set<int>& s) {
    EXPECT_EQ(v.minimum(), s.empty() ? -1 : *s.begin());
    EXPECT_EQ(v.maximum(), s.empty() ? -1 : *s.rbegin());
    for (int x = -2; x <= v.universe() + 1; ++x) {
        EXPECT_EQ(v.member(x), s.count(x) > 0) << "member(" << x << ")";
        EXPECT_EQ(v.successor(x), oracleSuccessor(s, x)) << "successor(" << x << ")";
        EXPECT_EQ(v.predecessor(x), oraclePredecessor(s, x)) << "predecessor(" << x << ")";
    }
}

}  // namespace

TEST(VanEmdeBoas, UniverseIsRoundedUpToAPowerOfTwo) {
    EXPECT_EQ(VanEmdeBoas(1).universe(), 2);
    EXPECT_EQ(VanEmdeBoas(2).universe(), 2);
    EXPECT_EQ(VanEmdeBoas(3).universe(), 4);
    EXPECT_EQ(VanEmdeBoas(100).universe(), 128);
    EXPECT_EQ(VanEmdeBoas(1 << 16).universe(), 1 << 16);
    EXPECT_THROW(VanEmdeBoas((1 << 30) + 1), std::invalid_argument);
}

TEST(VanEmdeBoas, SmallExample) {
    VanEmdeBoas v(16);
    EXPECT_TRUE(v.empty());
    EXPECT_EQ(v.minimum(), -1);
    for (int x : {2, 3, 4, 5, 7, 14, 15}) EXPECT_TRUE(v.insert(x));
    EXPECT_FALSE(v.insert(7));   // already there
    EXPECT_FALSE(v.insert(16));  // outside the universe
    EXPECT_FALSE(v.insert(-1));
    EXPECT_EQ(v.minimum(), 2);
    EXPECT_EQ(v.maximum(), 15);
    EXPECT_EQ(v.successor(5), 7);
    EXPECT_EQ(v.successor(7), 14);
    EXPECT_EQ(v.successor(15), -1);
    EXPECT_EQ(v.predecessor(7), 5);
    EXPECT_EQ(v.predecessor(2), -1);
    EXPECT_TRUE(v.del(7));
    EXPECT_FALSE(v.del(7));
    EXPECT_EQ(v.successor(5), 14);
    EXPECT_TRUE(v.del(2));  // delete the minimum
    EXPECT_EQ(v.minimum(), 3);
    EXPECT_TRUE(v.del(15));  // delete the maximum
    EXPECT_EQ(v.maximum(), 14);
}

TEST(VanEmdeBoas, ExhaustiveOperationsOnAUniverseOfEight) {
    // Insert every subset of {0..7} and check the full interface against std::set.
    for (unsigned mask = 0; mask < 256; ++mask) {
        VanEmdeBoas v(8);
        std::set<int> s;
        for (int x = 0; x < 8; ++x)
            if (mask >> x & 1u) {
                EXPECT_TRUE(v.insert(x));
                s.insert(x);
            }
        expectSameAsSet(v, s);
        for (int x = 0; x < 8; ++x) {
            VanEmdeBoas copyless(8);  // rebuild so that every deletion is tested on the same shape
            for (int y : s) copyless.insert(y);
            std::set<int> t = s;
            EXPECT_EQ(copyless.del(x), t.erase(x) == 1);
            expectSameAsSet(copyless, t);
        }
    }
}

TEST(VanEmdeBoas, RandomizedOperationsMatchStdSet) {
    for (int bits = 1; bits <= 10; ++bits) {
        testutil::Rng rng(200 + unsigned(bits));
        VanEmdeBoas v(1 << bits);
        std::set<int> s;
        for (int step = 0; step < 1500; ++step) {
            const int x = testutil::randInt(rng, 0, (1 << bits) - 1);
            switch (testutil::randInt(rng, 0, 3)) {
                case 0:
                case 1:
                    EXPECT_EQ(v.insert(x), s.insert(x).second);
                    break;
                case 2:
                    EXPECT_EQ(v.del(x), s.erase(x) == 1);
                    break;
                default:
                    EXPECT_EQ(v.member(x), s.count(x) > 0);
            }
            EXPECT_EQ(v.successor(x), oracleSuccessor(s, x));
            EXPECT_EQ(v.predecessor(x), oraclePredecessor(s, x));
            EXPECT_EQ(v.minimum(), s.empty() ? -1 : *s.begin());
            EXPECT_EQ(v.maximum(), s.empty() ? -1 : *s.rbegin());
        }
    }
}

TEST(VanEmdeBoas, LargeUniverseIsSparseInMemory) {
    // 2^30 keys would need 128 MB as a bit vector and 8 GB as a pointer array; the lazy clusters
    // make a handful of keys cheap.
    VanEmdeBoas v(1 << 30);
    const int keys[] = {0, 1, 12345678, (1 << 30) - 1, 999999999};
    for (int k : keys) EXPECT_TRUE(v.insert(k));
    EXPECT_EQ(v.minimum(), 0);
    EXPECT_EQ(v.maximum(), (1 << 30) - 1);
    EXPECT_EQ(v.successor(1), 12345678);
    EXPECT_EQ(v.successor(12345678), 999999999);
    EXPECT_EQ(v.predecessor(999999999), 12345678);
    EXPECT_TRUE(v.del(12345678));
    EXPECT_EQ(v.successor(1), 999999999);
    for (int k : keys) v.del(k);
    EXPECT_TRUE(v.empty());
}

TEST(VanEmdeBoas, MoveTransfersKeysAndLeavesAnEmptyUsableUniverse) {
    VanEmdeBoas source(16);
    for (int key : {1, 3, 9, 14}) source.insert(key);
    VanEmdeBoas moved(std::move(source));
    expectSameAsSet(moved, {1, 3, 9, 14});
    EXPECT_TRUE(source.empty());
    EXPECT_EQ(source.universe(), 2);
    EXPECT_FALSE(source.member(1));
    EXPECT_EQ(source.successor(0), -1);
    EXPECT_TRUE(source.insert(1));
    source = std::move(moved);
    expectSameAsSet(source, {1, 3, 9, 14});
    EXPECT_TRUE(moved.empty());
    EXPECT_TRUE(moved.insert(0));
    expectSameAsSet(moved, {0});
    VanEmdeBoas& alias = source;
    source = std::move(alias);
    expectSameAsSet(source, {1, 3, 9, 14});
}
