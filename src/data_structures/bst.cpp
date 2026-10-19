#include "introalgo/data_structures/bst.hpp"

#include <algorithm>
#include <limits>
#include <ostream>
#include <stack>
#include <utility>

namespace introalgo {

/*below is bst*/
TreeNode::TreeNode() : val(0), p(nullptr), left(nullptr), right(nullptr) {}
TreeNode::TreeNode(int va) : val(va), p(nullptr), left(nullptr), right(nullptr) {}
TreeNode::TreeNode(int va, TreeNode* par) : val(va), p(par), left(nullptr), right(nullptr) {}
TreeNode::TreeNode(int va, TreeNode* lef, TreeNode* righ)
    : val(va), p(nullptr), left(lef), right(righ) {
    if (left) left->p = this;
    if (right) right->p = this;
}
TreeNode::TreeNode(int va, TreeNode* par, TreeNode* lef, TreeNode* righ)
    : val(va), p(par), left(lef), right(righ) {
    if (left) left->p = this;
    if (right) right->p = this;
}

TreeNode::~TreeNode() {
    std::vector<TreeNode*> pending;
    if (left) pending.push_back(left);
    if (right) pending.push_back(right);
    left = right = nullptr;
    while (!pending.empty()) {
        TreeNode* n = pending.back();
        pending.pop_back();
        if (n->left) pending.push_back(n->left);
        if (n->right) pending.push_back(n->right);
        n->left = n->right = nullptr;  // so that ~TreeNode below does not recurse
        delete n;
    }
}

void TreeNode::insert(int value) {  // (The elements with same value in the left subtree)
    this->insert(new TreeNode(value));
}

void TreeNode::insert(TreeNode* node) {
    TreeNode* q = this;
    while (q) {
        if (node->val > q->val) {
            if (q->right) {
                q = q->right;
            } else {
                q->right = node;
                node->p = q;
                break;
            }
        } else {
            if (q->left) {
                q = q->left;
            } else {
                q->left = node;
                node->p = q;
                break;
            }
        }
    }
}

void TreeNode::inorder_traverse(std::ostream& os) const {
    for (int v : inorder()) os << v << std::endl;
    os << "done" << std::endl;
}

std::vector<int> TreeNode::inorder() const {
    std::vector<int> out;
    const TreeNode* root = this;
    std::stack<const TreeNode*> tree;
    while (root || tree.size()) {
        while (root) {
            tree.push(root);
            root = root->left;
        }
        root = tree.top();
        tree.pop();
        out.push_back(root->val);
        root = root->right;
    }
    return out;
}

TreeNode* TreeNode::getMin() {
    TreeNode* root = this;
    while (root->left) root = root->left;
    return root;
}

TreeNode* TreeNode::getMax() {
    TreeNode* root = this;
    while (root->right) root = root->right;
    return root;
}

TreeNode* TreeNode::getPredecessor(TreeNode* node) {
    if (!node) return nullptr;
    if (node->left) return node->left->getMax();
    // No left subtree: the predecessor is the closest ancestor of which we are in the right
    // subtree.
    TreeNode* up = node->p;
    while (up && node == up->left) {
        node = up;
        up = up->p;
    }
    return up;
}

TreeNode* TreeNode::getSuccessor(TreeNode* node) {
    if (!node) return nullptr;
    if (node->right) return node->right->getMin();
    TreeNode* up = node->p;
    while (up && node == up->right) {
        node = up;
        up = up->p;
    }
    return up;
}

TreeNode* TreeNode::search(int va) {
    TreeNode* root = this;
    while (root) {
        if (root->val == va) return root;
        if (va > root->val)
            root = root->right;
        else
            root = root->left;
    }
    return nullptr;
}

int TreeNode::getDepth() const {
    const TreeNode* root = this;
    int i = 1;
    while (root->p) {
        root = root->p;
        i++;
    }
    return i;
}

TreeNode* TreeNode::delNode() {
    TreeNode* replacement;
    if (!left) {
        replacement = right;
        alternative(right);
    } else if (!right) {
        replacement = left;
        alternative(left);
    } else {
        TreeNode* pred = left->getMax();  // has no right child
        if (pred != left) {
            pred->alternative(pred->left);  // lift pred's left subtree into pred's old place
            pred->left = left;
            left->p = pred;
        }
        alternative(pred);
        pred->right = right;
        right->p = pred;
        replacement = pred;
    }
    p = left = right = nullptr;
    return replacement;
}

void TreeNode::alternative(TreeNode* alt) {
    if (this->p) {
        if (this->p->left == this)
            this->p->left = alt;
        else
            this->p->right = alt;
    }
    if (alt) alt->p = this->p;
}

/*below is the owning tree*/
Bst::Bst() = default;
Bst::~Bst() = default;

TreeNode* Bst::find(int val) const {
    TreeNode* n = top.get();
    while (n && n->val != val) n = val > n->val ? n->right : n->left;
    return n;
}

void Bst::insert(int val) {
    if (!top)
        top.reset(new TreeNode(val));
    else
        top->insert(val);
    count++;
}

bool Bst::contains(int val) const {
    return find(val) != nullptr;
}

TreeNode* Bst::detach(TreeNode* node) {
    const bool wasRoot = node == top.get();
    TreeNode* replacement = node->delNode();
    if (wasRoot) {
        static_cast<void>(top.release());  // `node` is handed to the caller below
        top.reset(replacement);
    }
    return node;
}

bool Bst::erase(int val) {
    TreeNode* node = find(val);
    if (!node) return false;
    std::unique_ptr<TreeNode> gone(detach(node));
    count--;
    return true;
}

bool Bst::change(int va1, int va2) {
    TreeNode* node = find(va1);
    if (!node) return false;
    TreeNode* moved = detach(node);
    moved->val = va2;
    if (!top)
        top.reset(moved);
    else
        top->insert(moved);
    return true;
}

std::vector<int> Bst::inorder() const {
    return top ? top->inorder() : std::vector<int>{};
}

int Bst::height() const {
    int levels = 0;
    std::vector<const TreeNode*> level;
    if (top) level.push_back(top.get());
    while (!level.empty()) {
        levels++;
        std::vector<const TreeNode*> next;
        for (const TreeNode* n : level) {
            if (n->left) next.push_back(n->left);
            if (n->right) next.push_back(n->right);
        }
        level.swap(next);
    }
    return levels;
}

bool Bst::isValid() const {
    if (!top) return count == 0;
    if (top->p != nullptr) return false;
    // Each stack entry carries the open interval (lo, hi] the subtree must stay within.
    struct Frame {
        const TreeNode* node;
        long long lo, hi;  // lo < value <= hi
    };
    std::stack<Frame> todo;
    todo.push(
        {top.get(), std::numeric_limits<long long>::min(), std::numeric_limits<long long>::max()});
    std::size_t seen = 0;
    while (!todo.empty()) {
        const Frame f = todo.top();
        todo.pop();
        ++seen;
        if (!(f.lo < f.node->val && f.node->val <= f.hi)) return false;
        if (f.node->left) {
            if (f.node->left->p != f.node) return false;
            todo.push({f.node->left, f.lo, f.node->val});
        }
        if (f.node->right) {
            if (f.node->right->p != f.node) return false;
            todo.push({f.node->right, f.node->val, f.hi});
        }
    }
    return seen == count;
}

}  // namespace introalgo
