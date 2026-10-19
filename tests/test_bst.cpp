#include <gtest/gtest.h>

#include <algorithm>
#include <set>
#include <sstream>
#include <vector>

#include "introalgo/data_structures/bst.hpp"
#include "test_utils.hpp"

using introalgo::Bst;
using introalgo::TreeNode;

TEST(TreeNode, InsertSearchAndInorder) {
    TreeNode root(10);
    for (int v : {33, 44, 1323, 42, 99, 0, 1, 4, 29, 4, 5}) root.insert(v);
    EXPECT_EQ(root.inorder(), (std::vector<int>{0, 1, 4, 4, 5, 10, 29, 33, 42, 44, 99, 1323}));
    EXPECT_NE(root.search(29), nullptr);
    EXPECT_EQ(root.search(30), nullptr);  // the original printed a message here
    EXPECT_EQ(root.getMin()->val, 0);
    EXPECT_EQ(root.getMax()->val, 1323);
}

TEST(TreeNode, InorderTraversePrintsLikeTheOriginal) {
    TreeNode root(2);
    root.insert(1);
    root.insert(3);
    std::ostringstream out;
    root.inorder_traverse(out);
    EXPECT_EQ(out.str(), "1\n2\n3\ndone\n");
}

TEST(TreeNode, PredecessorAndSuccessorUseParentLinksWhenThereIsNoSubtree) {
    // The original dereferenced left/right unconditionally and crashed for leaves.
    TreeNode root(10);
    for (int v : {5, 15, 3, 7, 12, 20}) root.insert(v);
    TreeNode* seven = root.search(7);
    EXPECT_EQ(root.getPredecessor(seven)->val, 5);
    EXPECT_EQ(root.getSuccessor(seven)->val, 10);
    TreeNode* twelve = root.search(12);
    EXPECT_EQ(root.getPredecessor(twelve)->val, 10);
    EXPECT_EQ(root.getSuccessor(twelve)->val, 15);
    EXPECT_EQ(root.getPredecessor(root.search(3)), nullptr);
    EXPECT_EQ(root.getSuccessor(root.search(20)), nullptr);
    EXPECT_EQ(root.getSuccessor(&root)->val, 12);
    EXPECT_EQ(root.getPredecessor(nullptr), nullptr);
}

TEST(TreeNode, GetDepthReturnsTheNumberOfLevels) {
    // getDepth() had no return statement in the original.
    TreeNode root(10);
    for (int v : {5, 15, 3}) root.insert(v);
    EXPECT_EQ(root.getDepth(), 1);
    EXPECT_EQ(root.search(5)->getDepth(), 2);
    EXPECT_EQ(root.search(3)->getDepth(), 3);
}

TEST(TreeNode, DelNodeLeafOneChildAndTwoChildren) {
    TreeNode root(10);
    for (int v : {5, 15, 3, 7, 12, 20, 6}) root.insert(v);
    // leaf
    TreeNode* twenty = root.search(20);
    EXPECT_EQ(twenty->delNode(), nullptr);  // nothing takes the leaf's place
    delete twenty;                          // the detached node belongs to the caller
    EXPECT_EQ(root.inorder(), (std::vector<int>{3, 5, 6, 7, 10, 12, 15}));
    // one child: 15 -> replaced by 12
    TreeNode* fifteen = root.search(15);
    TreeNode* replacement = fifteen->delNode();
    EXPECT_EQ(replacement->val, 12);
    EXPECT_EQ(root.right, replacement);
    EXPECT_EQ(replacement->p, &root);
    EXPECT_EQ(fifteen->p, nullptr);
    delete fifteen;
    // two children: 5 -> replaced by its predecessor 3
    TreeNode* five = root.search(5);
    TreeNode* pred = five->delNode();
    EXPECT_EQ(pred->val, 3);
    delete five;
    EXPECT_EQ(root.inorder(), (std::vector<int>{3, 6, 7, 10, 12}));
}

TEST(Bst, EraseTheRootRepeatedly) {
    Bst t;
    for (int v : {10, 5, 15, 3, 7}) t.insert(v);
    for (int expectedSize = 5; expectedSize > 0; --expectedSize) {
        ASSERT_EQ(t.size(), std::size_t(expectedSize));
        ASSERT_TRUE(t.erase(t.root()->val));
        ASSERT_TRUE(t.isValid());
    }
    EXPECT_TRUE(t.empty());
    EXPECT_EQ(t.root(), nullptr);
    EXPECT_FALSE(t.erase(1));
    t.insert(2);  // usable again after being emptied
    EXPECT_TRUE(t.contains(2));
}

TEST(Bst, ChangeMovesTheNodeToItsNewPlace) {
    Bst t;
    for (int v : {10, 33, 44, 1323, 42, 99, 0, 1, 4, 29, 4, 5}) t.insert(v);
    EXPECT_TRUE(t.change(1323, 11));
    EXPECT_EQ(t.inorder(), (std::vector<int>{0, 1, 4, 4, 5, 10, 11, 29, 33, 42, 44, 99}));
    EXPECT_FALSE(t.change(777, 1));
    EXPECT_TRUE(t.change(10, 1000));  // changing the root
    EXPECT_TRUE(t.isValid());
    EXPECT_EQ(t.size(), 12u);
    EXPECT_EQ(t.inorder().back(), 1000);
}

TEST(Bst, DuplicatesGoLeft) {
    Bst t;
    for (int i = 0; i < 5; ++i) t.insert(7);
    EXPECT_EQ(t.height(), 5);  // a chain to the left
    EXPECT_TRUE(t.isValid());
    EXPECT_TRUE(t.erase(7));
    EXPECT_EQ(t.size(), 4u);
}

TEST(Bst, DegenerateTreesDoNotOverflowTheStack) {
    Bst t;
    // Sorted input: a 20000-node path (the inserts themselves are O(n^2), the destructor must not
    // recurse once per level).
    for (int i = 0; i < 20000; ++i) t.insert(i);
    EXPECT_EQ(t.height(), 20000);
    EXPECT_TRUE(t.isValid());
    // Destruction of the whole chain happens at the end of the test.
}

TEST(Bst, RandomizedOperationsMatchMultiset) {
    testutil::Rng rng(31);
    Bst t;
    std::multiset<int> oracle;
    for (int step = 0; step < 6000; ++step) {
        const int v = testutil::randInt(rng, 0, 60);
        switch (testutil::randInt(rng, 0, 3)) {
            case 0:
            case 1:
                t.insert(v);
                oracle.insert(v);
                break;
            case 2: {
                auto it = oracle.find(v);
                EXPECT_EQ(t.erase(v), it != oracle.end());
                if (it != oracle.end()) oracle.erase(it);
                break;
            }
            default: {
                const int w = testutil::randInt(rng, 0, 60);
                auto it = oracle.find(v);
                EXPECT_EQ(t.change(v, w), it != oracle.end());
                if (it != oracle.end()) {
                    oracle.erase(it);
                    oracle.insert(w);
                }
            }
        }
        ASSERT_EQ(t.size(), oracle.size());
        if (step % 25 == 0) {
            ASSERT_TRUE(t.isValid()) << "step " << step;
            ASSERT_EQ(t.inorder(), std::vector<int>(oracle.begin(), oracle.end()));
        }
        EXPECT_EQ(t.contains(v), oracle.count(v) > 0);
    }
}
