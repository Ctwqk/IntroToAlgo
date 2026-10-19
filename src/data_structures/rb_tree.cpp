#include "introalgo/data_structures/rb_tree.hpp"

#include <algorithm>

namespace introalgo {

enum class Color { Red, Black };

struct RbTree::Node {
    int key = 0;
    Color color = Color::Black;
    Node* left = nullptr;
    Node* right = nullptr;
    Node* parent = nullptr;
};

RbTree::RbTree() : nil(new Node), root(nullptr) {
    nil->left = nil->right = nil->parent = nil;
    root = nil;
}

RbTree::~RbTree() {
    std::vector<Node*> pending;
    if (root != nil) pending.push_back(root);
    while (!pending.empty()) {
        Node* n = pending.back();
        pending.pop_back();
        if (n->left != nil) pending.push_back(n->left);
        if (n->right != nil) pending.push_back(n->right);
        delete n;
    }
    delete nil;
}

void RbTree::leftRotate(Node* x) {
    Node* y = x->right;
    x->right = y->left;
    if (y->left != nil) y->left->parent = x;
    y->parent = x->parent;
    if (x->parent == nil)
        root = y;
    else if (x == x->parent->left)
        x->parent->left = y;
    else
        x->parent->right = y;
    y->left = x;
    x->parent = y;
}

void RbTree::rightRotate(Node* x) {
    Node* y = x->left;
    x->left = y->right;
    if (y->right != nil) y->right->parent = x;
    y->parent = x->parent;
    if (x->parent == nil)
        root = y;
    else if (x == x->parent->right)
        x->parent->right = y;
    else
        x->parent->left = y;
    y->right = x;
    x->parent = y;
}

void RbTree::insert(int key) {
    Node* z = new Node;
    z->key = key;
    Node* y = nil;
    Node* x = root;
    while (x != nil) {
        y = x;
        x = key < x->key ? x->left : x->right;
    }
    z->parent = y;
    if (y == nil)
        root = z;
    else if (key < y->key)
        y->left = z;
    else
        y->right = z;
    z->left = z->right = nil;
    z->color = Color::Red;
    insertFixup(z);
    ++count;
}

void RbTree::insertFixup(Node* z) {
    while (z->parent->color == Color::Red) {
        if (z->parent == z->parent->parent->left) {
            Node* u = z->parent->parent->right;  // uncle
            if (u->color == Color::Red) {
                z->parent->color = Color::Black;
                u->color = Color::Black;
                z->parent->parent->color = Color::Red;
                z = z->parent->parent;
            } else {
                if (z == z->parent->right) {
                    z = z->parent;
                    leftRotate(z);
                }
                z->parent->color = Color::Black;
                z->parent->parent->color = Color::Red;
                rightRotate(z->parent->parent);
            }
        } else {
            Node* u = z->parent->parent->left;
            if (u->color == Color::Red) {
                z->parent->color = Color::Black;
                u->color = Color::Black;
                z->parent->parent->color = Color::Red;
                z = z->parent->parent;
            } else {
                if (z == z->parent->left) {
                    z = z->parent;
                    rightRotate(z);
                }
                z->parent->color = Color::Black;
                z->parent->parent->color = Color::Red;
                leftRotate(z->parent->parent);
            }
        }
    }
    root->color = Color::Black;
}

void RbTree::transplant(Node* u, Node* v) {
    if (u->parent == nil)
        root = v;
    else if (u == u->parent->left)
        u->parent->left = v;
    else
        u->parent->right = v;
    v->parent = u->parent;  // also when v is the sentinel: deleteFixup relies on it
}

RbTree::Node* RbTree::minimum(Node* x) const {
    while (x->left != nil) x = x->left;
    return x;
}

RbTree::Node* RbTree::find(int key) const {
    Node* x = root;
    while (x != nil && x->key != key) x = key < x->key ? x->left : x->right;
    return x;
}

bool RbTree::contains(int key) const { return find(key) != nil; }

bool RbTree::erase(int key) {
    Node* z = find(key);
    if (z == nil) return false;
    Node* y = z;
    Color yOriginal = y->color;
    Node* x;
    if (z->left == nil) {
        x = z->right;
        transplant(z, z->right);
    } else if (z->right == nil) {
        x = z->left;
        transplant(z, z->left);
    } else {
        y = minimum(z->right);
        yOriginal = y->color;
        x = y->right;
        if (y->parent == z) {
            x->parent = y;
        } else {
            transplant(y, y->right);
            y->right = z->right;
            y->right->parent = y;
        }
        transplant(z, y);
        y->left = z->left;
        y->left->parent = y;
        y->color = z->color;
    }
    delete z;
    if (yOriginal == Color::Black) deleteFixup(x);
    --count;
    return true;
}

void RbTree::deleteFixup(Node* x) {
    while (x != root && x->color == Color::Black) {
        if (x == x->parent->left) {
            Node* w = x->parent->right;  // sibling
            if (w->color == Color::Red) {
                w->color = Color::Black;
                x->parent->color = Color::Red;
                leftRotate(x->parent);
                w = x->parent->right;
            }
            if (w->left->color == Color::Black && w->right->color == Color::Black) {
                w->color = Color::Red;
                x = x->parent;
            } else {
                if (w->right->color == Color::Black) {
                    w->left->color = Color::Black;
                    w->color = Color::Red;
                    rightRotate(w);
                    w = x->parent->right;
                }
                w->color = x->parent->color;
                x->parent->color = Color::Black;
                w->right->color = Color::Black;
                leftRotate(x->parent);
                x = root;
            }
        } else {
            Node* w = x->parent->left;
            if (w->color == Color::Red) {
                w->color = Color::Black;
                x->parent->color = Color::Red;
                rightRotate(x->parent);
                w = x->parent->left;
            }
            if (w->right->color == Color::Black && w->left->color == Color::Black) {
                w->color = Color::Red;
                x = x->parent;
            } else {
                if (w->left->color == Color::Black) {
                    w->right->color = Color::Black;
                    w->color = Color::Red;
                    leftRotate(w);
                    w = x->parent->left;
                }
                w->color = x->parent->color;
                x->parent->color = Color::Black;
                w->left->color = Color::Black;
                rightRotate(x->parent);
                x = root;
            }
        }
    }
    x->color = Color::Black;
}

std::vector<int> RbTree::inorder() const {
    std::vector<int> out;
    std::vector<const Node*> stack;
    const Node* n = root;
    while (n != nil || !stack.empty()) {
        while (n != nil) {
            stack.push_back(n);
            n = n->left;
        }
        n = stack.back();
        stack.pop_back();
        out.push_back(n->key);
        n = n->right;
    }
    return out;
}

int RbTree::height() const {
    int levels = 0;
    std::vector<const Node*> level;
    if (root != nil) level.push_back(root);
    while (!level.empty()) {
        ++levels;
        std::vector<const Node*> next;
        for (const Node* n : level) {
            if (n->left != nil) next.push_back(n->left);
            if (n->right != nil) next.push_back(n->right);
        }
        level.swap(next);
    }
    return levels;
}

bool RbTree::validate(std::string* why) const {
    auto fail = [&](const char* msg) {
        if (why) *why = msg;
        return false;
    };
    if (nil->color != Color::Black) return fail("sentinel is not black");
    if (root == nil) return count == 0 ? true : fail("empty tree with non-zero size");
    if (root->color != Color::Black) return fail("root is red");
    if (root->parent != nil) return fail("root has a parent");

    // Iterative post-order walk computing black heights.
    struct Frame {
        const Node* node;
        int state;  // 0: enter, 1: children done
    };
    std::vector<Frame> stack{{root, 0}};
    std::vector<int> heights;  // black height of the most recently finished subtrees
    std::size_t nodes = 0;
    while (!stack.empty()) {
        Frame& f = stack.back();
        const Node* n = f.node;
        if (n == nil) {
            heights.push_back(1);  // the sentinel counts as one black node
            stack.pop_back();
            continue;
        }
        if (f.state == 0) {
            ++nodes;
            if (n->left != nil && n->left->parent != n) return fail("broken parent link (left)");
            if (n->right != nil && n->right->parent != n) return fail("broken parent link (right)");
            if (n->color == Color::Red &&
                (n->left->color == Color::Red || n->right->color == Color::Red))
                return fail("red node with a red child");
            if (n->left != nil && n->left->key > n->key) return fail("left child larger than parent");
            if (n->right != nil && n->right->key < n->key) return fail("right child smaller than parent");
            f.state = 1;
            const Node* l = n->left;
            const Node* r = n->right;
            stack.push_back({r, 0});
            stack.push_back({l, 0});
        } else {
            const int right = heights.back();
            heights.pop_back();
            const int left = heights.back();
            heights.pop_back();
            if (left != right) return fail("paths with different numbers of black nodes");
            heights.push_back(left + (n->color == Color::Black ? 1 : 0));
            stack.pop_back();
        }
    }
    if (nodes != count) return fail("size does not match the number of nodes");
    const auto order = inorder();
    if (!std::is_sorted(order.begin(), order.end())) return fail("in-order traversal is not sorted");
    return true;
}

}  // namespace introalgo
