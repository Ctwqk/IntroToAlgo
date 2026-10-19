#include <gtest/gtest.h>

#include <algorithm>
#include <map>
#include <set>
#include <utility>
#include <vector>

#include "introalgo/data_structures/chain_hash.hpp"
#include "test_utils.hpp"

using introalgo::ChainHash;

namespace {

// Every value must sit in the chain of its own bucket and the chains must hold exactly the
// multiset of values we expect.
void expectInvariants(const ChainHash& h, const std::multiset<int>& expected) {
    std::multiset<int> stored;
    int total = 0;
    for (int b = 0; b < h.bucketCount(); ++b)
        for (int v : h.bucket(b)) {
            EXPECT_EQ(h.Hash_func(v), b) << "value " << v << " is in the wrong bucket";
            stored.insert(v);
            ++total;
        }
    EXPECT_EQ(total, h.size());
    EXPECT_EQ(stored, expected);
}

}  // namespace

TEST(ChainHash, InsertSearchDelete) {
    ChainHash h;
    for (int v : {5, 15, 25, 7}) h.insert(v);  // 5, 15 and 25 collide in a table of 10
    EXPECT_EQ(h.bucket(5), (std::vector<int>{5, 15, 25}));
    EXPECT_NE(h.search(15), nullptr);
    EXPECT_EQ(h.search(15)->val, 15);
    EXPECT_EQ(h.search(35), nullptr);

    EXPECT_TRUE(h.delete_val(15));  // the original never removed anything
    EXPECT_EQ(h.search(15), nullptr);
    EXPECT_EQ(h.bucket(5), (std::vector<int>{5, 25}));
    EXPECT_FALSE(h.delete_val(15));
    EXPECT_EQ(h.size(), 3);
}

TEST(ChainHash, DeleteHeadMiddleAndTail) {
    ChainHash h(1);  // single bucket: one long chain
    for (int v : {1, 2, 3, 4}) h.insert(v);
    EXPECT_TRUE(h.delete_val(1));
    EXPECT_TRUE(h.delete_val(4));
    EXPECT_TRUE(h.delete_val(3));
    EXPECT_EQ(h.bucket(0), (std::vector<int>{2}));
    EXPECT_TRUE(h.delete_val(2));
    EXPECT_TRUE(h.bucket(0).empty());
    EXPECT_EQ(h.size(), 0);
}

TEST(ChainHash, DuplicatesAreKeptAndRemovedOneAtATime) {
    ChainHash h;
    h.insert(3);
    h.insert(3);
    EXPECT_EQ(h.size(), 2);
    EXPECT_TRUE(h.delete_val(3));
    EXPECT_NE(h.search(3), nullptr);
    EXPECT_TRUE(h.delete_val(3));
    EXPECT_EQ(h.search(3), nullptr);
}

TEST(ChainHash, NegativeKeysHashIntoRange) {
    ChainHash h(7);
    for (int v : {-1, -7, -8, 0, 6}) {
        EXPECT_GE(h.Hash_func(v), 0);
        EXPECT_LT(h.Hash_func(v), 7);
        h.insert(v);
    }
    EXPECT_NE(h.search(-8), nullptr);
    EXPECT_TRUE(h.delete_val(-8));
}

TEST(ChainHash, TableSizeIsHonoured) {
    // The original ignored its size argument and always reduced modulo 10.
    ChainHash h(3);
    EXPECT_EQ(h.bucketCount(), 3);
    EXPECT_EQ(h.Hash_func(7), 1);
    EXPECT_THROW(ChainHash(0), std::invalid_argument);
    EXPECT_THROW(ChainHash(-4), std::invalid_argument);
}

TEST(ChainHash, ModifyMovesTheValueToItsNewBucket) {
    ChainHash h;
    h.insert(12);
    EXPECT_TRUE(h.modify(12, 49));
    EXPECT_EQ(h.search(12), nullptr);
    EXPECT_NE(h.search(49), nullptr);
    EXPECT_EQ(h.bucket(9), (std::vector<int>{49}));
    EXPECT_FALSE(h.modify(1000, 1));
    EXPECT_EQ(h.size(), 1);
}

TEST(ChainHash, SortedInsertKeepsChainsOrdered) {
    ChainHash inc(2), dec(2);
    for (int v : {8, 2, 6, 4, 0, 10, 4}) {
        inc.sortedInsert(v);                // default: increasing
        dec.sortedInsert(v, "decreasing");  // all even values -> same bucket
    }
    EXPECT_EQ(inc.bucket(0), (std::vector<int>{0, 2, 4, 4, 6, 8, 10}));
    EXPECT_EQ(dec.bucket(0), (std::vector<int>{10, 8, 6, 4, 4, 2, 0}));
    EXPECT_THROW(inc.sortedInsert(1, "sideways"), std::invalid_argument);
    EXPECT_EQ(inc.size(), 7);
}

TEST(ChainHash, CopyIsDeepAndIndependent) {
    ChainHash a;
    for (int v : {1, 11, 2}) a.insert(v);
    ChainHash b(a);
    a.delete_val(1);
    EXPECT_NE(b.search(1), nullptr);
    EXPECT_EQ(b.bucket(1), (std::vector<int>{1, 11}));
    ChainHash c;
    c = b;
    b.delete_val(11);
    EXPECT_NE(c.search(11), nullptr);
    ChainHash d(std::move(c));
    EXPECT_EQ(d.size(), 3);
}

TEST(ChainHash, RandomizedOperationsMatchMultiset) {
    testutil::Rng rng(24);
    ChainHash h(13);
    std::multiset<int> oracle;
    for (int step = 0; step < 5000; ++step) {
        const int v = testutil::randInt(rng, -40, 40);
        switch (testutil::randInt(rng, 0, 4)) {
            case 0:
            case 1:
                h.insert(v);
                oracle.insert(v);
                break;
            case 2: {
                auto it = oracle.find(v);
                EXPECT_EQ(h.delete_val(v), it != oracle.end());
                if (it != oracle.end()) oracle.erase(it);
                break;
            }
            case 3:
                EXPECT_EQ(h.search(v) != nullptr, oracle.count(v) > 0);
                break;
            default: {
                const int w = testutil::randInt(rng, -40, 40);
                auto it = oracle.find(v);
                EXPECT_EQ(h.modify(v, w), it != oracle.end());
                if (it != oracle.end()) {
                    oracle.erase(it);
                    oracle.insert(w);
                }
            }
        }
        if (step % 100 == 0) expectInvariants(h, oracle);
    }
    expectInvariants(h, oracle);
}

TEST(ChainHash, MovedFromTablesRemainUsable) {
    ChainHash source(7);
    source.insert(12);
    source.insert(-2);
    ChainHash moved(std::move(source));
    expectInvariants(moved, {12, -2});
    EXPECT_EQ(source.size(), 0);
    EXPECT_DOUBLE_EQ(source.loadFactor(), 0);
    EXPECT_EQ(source.search(12), nullptr);
    EXPECT_FALSE(source.delete_val(12));
    source.sortedInsert(5);
    expectInvariants(source, {5});
    source = std::move(moved);
    expectInvariants(source, {12, -2});
    moved.insert(6);
    expectInvariants(moved, {6});
    ChainHash& alias = source;
    source = std::move(alias);
    expectInvariants(source, {12, -2});
}
