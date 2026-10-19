#pragma once

#include <cstddef>
#include <iosfwd>
#include <memory>
#include <vector>

namespace introalgo {

/// Node of a binary search tree with parent pointers.
///
/// Elements equal to a node go to its LEFT subtree ("the elements with same value in the left
/// subtree" in the original), so for every node: left subtree <= val < right subtree.
///
/// A TreeNode owns its subtree: destroying a node destroys everything below it (iteratively, so
/// even a degenerate tree of a million nodes is safe).  insert(TreeNode*) therefore needs a
/// heap-allocated node.
class TreeNode {
public:
    int val;
    TreeNode *p, *left, *right;

    TreeNode();
    explicit TreeNode(int va);
    TreeNode(int va, TreeNode* par);
    TreeNode(int va, TreeNode* lef, TreeNode* righ);
    TreeNode(int va, TreeNode* par, TreeNode* lef, TreeNode* righ);
    ~TreeNode();
    TreeNode(const TreeNode&) = delete;
    TreeNode& operator=(const TreeNode&) = delete;

    void insert(int value);
    void insert(TreeNode* node);

    /// Prints the values of this subtree in order, one per line, followed by "done".
    void inorder_traverse(std::ostream& os) const;
    /// Values of this subtree in order.
    std::vector<int> inorder() const;

    TreeNode* getMin();
    TreeNode* getMax();
    /// In-order predecessor / successor of `node` in the whole tree (nullptr if there is none).
    TreeNode* getPredecessor(TreeNode* node);
    TreeNode* getSuccessor(TreeNode* node);

    /// Some node with this value in the subtree, or nullptr.
    TreeNode* search(int value);
    /// Number of nodes on the path from the tree's root to this node (the root has depth 1).
    int getDepth() const;

    /// Unlinks this node from its tree and returns the node that now takes its place (nullptr if
    /// the subtree became empty).  The detached node has no parent and no children and is
    /// owned by the caller (delete it, or insert it again).  A node with two children is
    /// replaced by its in-order predecessor, which keeps the "equal values on the left" rule.
    TreeNode* delNode();
    TreeNode* delNode(TreeNode* node) { return node->delNode(); }

    /// Makes `alt` take this node's place under this node's parent.
    void alternative(TreeNode* alt);
};

/// A binary search tree that owns its root; the usual set-like interface on top of TreeNode.
/// Duplicates are stored (multiset).  Operations are O(height), which is O(n) in the worst case.
class Bst {
public:
    Bst();
    ~Bst();

    void insert(int value);
    /// Removes one occurrence of `val`; false if it is not present.
    bool erase(int val);
    bool contains(int val) const;
    /// Gives one occurrence of `va1` the value `va2` (move = detach, change value, re-insert).
    bool change(int va1, int va2);

    std::vector<int> inorder() const;
    std::size_t size() const { return count; }
    bool empty() const { return count == 0; }
    /// Number of levels (0 for the empty tree).
    int height() const;
    const TreeNode* root() const { return top.get(); }

    /// Checks the BST ordering and every parent pointer.  Meant for tests.
    bool isValid() const;

private:
    std::unique_ptr<TreeNode> top;
    std::size_t count = 0;

    TreeNode* find(int val) const;
    TreeNode* detach(TreeNode* node);
};

}  // namespace introalgo
