#pragma once

#include <cstddef>
#include <string>
#include <vector>

namespace introalgo {

/// Red-black tree (CLRS chapter 13) holding ints; duplicates are allowed (multiset).
///
/// The original bst.cpp stopped at a half-written `RbNode` (a shared static Nil sentinel,
/// insert fix-up with assignments inside conditions, no working rotations).  This is a clean
/// implementation of the same design: nodes with parent pointers, a sentinel Nil per tree,
/// rotations, insert/delete fix-ups.  insert, erase and contains are O(log n).
class RbTree {
public:
    RbTree();
    ~RbTree();
    RbTree(const RbTree&) = delete;
    RbTree& operator=(const RbTree&) = delete;

    void insert(int key);
    /// Removes one occurrence; false if absent.
    bool erase(int key);
    bool contains(int key) const;

    std::size_t size() const { return count; }
    std::vector<int> inorder() const;
    /// Number of levels (0 for the empty tree); at most 2 * log2(n + 1).
    int height() const;

    /// Verifies every red-black property plus the search-tree order and parent links.  On failure
    /// returns false and, if `why` is given, says which rule broke.  Meant for tests.
    bool validate(std::string* why = nullptr) const;

private:
    struct Node;
    Node* nil;
    Node* root;
    std::size_t count = 0;

    void leftRotate(Node* x);
    void rightRotate(Node* x);
    void insertFixup(Node* z);
    void transplant(Node* u, Node* v);
    void deleteFixup(Node* x);
    Node* minimum(Node* x) const;
    Node* find(int key) const;
};

}  // namespace introalgo
