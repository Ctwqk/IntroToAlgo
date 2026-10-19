// Original demo of bst.cpp (test_bst / test_RB): build a BST, change a value, then the same
// values in a red-black tree.
#include <iostream>
#include <vector>

#include "introalgo/data_structures/bst.hpp"
#include "introalgo/data_structures/rb_tree.hpp"

int main() {
    std::vector<int> values{10, 33, 44, 1323, 42, 99, 0, 1, 4, 29, 4, 5};
    introalgo::Bst bst;
    for (int v : values) bst.insert(v);
    std::cout << "BST in order:";
    for (int v : bst.inorder()) std::cout << " " << v;
    std::cout << "  (height " << bst.height() << ")" << std::endl;
    bst.change(1323, 11);
    std::cout << "after change(1323, 11):";
    for (int v : bst.inorder()) std::cout << " " << v;
    std::cout << std::endl;

    introalgo::RbTree rb;
    for (int v : values) rb.insert(v);
    std::cout << "red-black tree in order:";
    for (int v : rb.inorder()) std::cout << " " << v;
    std::cout << "  (height " << rb.height() << ", valid: " << (rb.validate() ? "yes" : "no") << ")"
              << std::endl;
}
