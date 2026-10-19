#include <gtest/gtest.h>

#include <algorithm>
#include <functional>
#include <queue>
#include <sstream>
#include <vector>

#include "introalgo/data_structures/max_heap.hpp"
#include "test_utils.hpp"

using introalgo::Heap;
using introalgo::HeapEntry;

TEST(MaxHeap, OriginalDemoValues) {
    std::vector<int> values{10, 33,   44,  1323, 42,  99,  0,   1,   4,   29, 34,
                            63546, 478, 4452, 464, 245, 243, 345, 2345, 4, 5};
    Heap h;
    for (int v : values) {
        h.insert(HeapEntry(v));
        ASSERT_TRUE(h.isMaxHeap());
    }
    EXPECT_EQ(h.size(), 21);
    EXPECT_EQ(h.peek().getKey(), 63546);
    std::ostringstream out;
    h.showHeap(out);
    // One line per level: 1, 2, 4, 8 and 6 elements.
    std::istringstream lines(out.str());
    std::string line;
    std::vector<std::size_t> counts;
    while (std::getline(lines, line)) {
        std::istringstream words(line);
        int n = 0, x;
        while (words >> x) ++n;
        counts.push_back(std::size_t(n));
    }
    EXPECT_EQ(counts, (std::vector<std::size_t>{1, 2, 4, 8, 6}));
}

TEST(MaxHeap, SiftUpUsesTheZeroBasedParent) {
    // In the original, insert() moved an element up using pos/2 as the parent (a 1-based formula)
    // while heapify() used 2i+1 / 2i+2 children.  This sequence left a larger key below a smaller
    // one in the original.
    Heap h;
    for (int v : {5, 9, 15, 7, 6, 3, 15}) h.insert(v);
    EXPECT_TRUE(h.isMaxHeap());
    std::vector<int> drained;
    for (; !h.empty(); h.pop()) drained.push_back(h.peek().getKey());
    EXPECT_EQ(drained, (std::vector<int>{15, 15, 9, 7, 6, 5, 3}));
}

TEST(MaxHeap, PopReturnsKeysInDescendingOrder) {
    testutil::Rng rng(21);
    auto values = testutil::randVector(rng, 500, -1000, 1000);
    Heap h;
    for (int v : values) h.insert(v);
    std::sort(values.begin(), values.end(), std::greater<int>());
    for (int v : values) {
        ASSERT_FALSE(h.empty());
        EXPECT_EQ(h.peek().getKey(), v);
        h.pop();
        ASSERT_TRUE(h.isMaxHeap());
    }
    EXPECT_TRUE(h.empty());
}

TEST(MaxHeap, EmptyHeapThrowsInsteadOfReturningGarbage) {
    Heap h;
    EXPECT_THROW(h.peek(), std::out_of_range);
    EXPECT_THROW(h.pop(), std::out_of_range);
    h.insert(7);
    h.pop();
    EXPECT_THROW(h.pop(), std::out_of_range);
    std::ostringstream out;
    h.showHeap(out);  // log2(0) in the original
    EXPECT_EQ(out.str(), "");
}

TEST(MaxHeap, GrowsBeyondTheInitialCapacity) {
    // The original had a fixed array of `cap` entries and silently overflowed it.
    Heap h(4);
    for (int i = 0; i < 10000; ++i) h.insert(i);
    EXPECT_EQ(h.size(), 10000);
    EXPECT_EQ(h.peek().getKey(), 9999);
    EXPECT_TRUE(h.isMaxHeap());
}

TEST(MaxHeap, ConstructFromVectorBuildsAHeap) {
    testutil::Rng rng(22);
    for (int iter = 0; iter < 100; ++iter) {
        auto values = testutil::randVector(rng, std::size_t(testutil::randInt(rng, 0, 200)), -50, 50);
        Heap h(values);
        EXPECT_TRUE(h.isMaxHeap());
        EXPECT_EQ(h.size(), int(values.size()));
        std::sort(values.begin(), values.end(), std::greater<int>());
        for (int v : values) {
            EXPECT_EQ(h.peek().getKey(), v);
            h.pop();
        }
    }
}

TEST(MaxHeap, HeapifyByEntry) {
    Heap h(std::vector<int>{5, 3, 8});
    EXPECT_TRUE(h.heapify(HeapEntry(3)));
    EXPECT_FALSE(h.heapify(HeapEntry(99)));
    EXPECT_TRUE(h.isMaxHeap());
    h.heapify(-1);   // out of range positions are ignored
    h.heapify(100);
    EXPECT_TRUE(h.isMaxHeap());
}

TEST(MaxHeap, SatelliteDataTravelsWithTheKey) {
    Heap h;
    h.insert(HeapEntry(5, 50));
    h.insert(HeapEntry(9, 90));
    h.insert(HeapEntry(1, 10));
    EXPECT_EQ(h.peek().getOtherData(), 90);
    h.pop();
    EXPECT_EQ(h.peek().getOtherData(), 50);
}

TEST(MaxHeap, RandomizedOperationsMatchPriorityQueue) {
    testutil::Rng rng(23);
    Heap h(8);
    std::priority_queue<int> oracle;
    for (int step = 0; step < 20000; ++step) {
        if (oracle.empty() || testutil::randInt(rng, 0, 2) != 0) {
            const int v = testutil::randInt(rng, -100, 100);
            h.insert(v);
            oracle.push(v);
        } else {
            ASSERT_EQ(h.peek().getKey(), oracle.top());
            h.pop();
            oracle.pop();
        }
        ASSERT_EQ(h.size(), int(oracle.size()));
        if (step % 100 == 0) {
            ASSERT_TRUE(h.isMaxHeap());
        }
    }
}
