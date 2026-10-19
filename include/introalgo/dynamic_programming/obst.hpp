#pragma once

#include <iosfwd>
#include <vector>

namespace introalgo {

/// Optimal binary search tree (CLRS 15.5).
///
/// There are n keys k1 < ... < kn searched with probabilities p1..pn and
/// n + 1 "dummy keys" d0..dn (unsuccessful searches) with probabilities
/// q0..qn.  The dynamic program finds the BST with the smallest expected search
/// cost.  O(n^3) time, O(n^2) space.
///
/// As in the original program the class stores `size` = n + 1 and uses 1-based
/// indices for keys in the tables e (expected cost), w (probability mass) and
/// root.
class Obst {
public:
    /// `p` holds n key probabilities, `q` holds n + 1 dummy-key probabilities.
    Obst(const std::vector<double>& p, const std::vector<double>& q);

    /// Runs the dynamic program (the original called it find_obst2).
    void find_obst2();

    /// Expected search cost of the optimal tree (valid after find_obst2()).
    double expectedCost() const;

    /// Number of real keys.
    int keyCount() const { return size - 1; }

    /// 1-based index of the root of the optimal subtree for keys i..j (1 <= i <= j <= n).
    int rootOf(int i, int j) const;

    /// Depth of every key in the optimal tree (root = 0), indexed by k1..kn -> [0, n).
    std::vector<int> keyDepths() const;
    /// Depth of every dummy key d0..dn in the optimal tree (a leaf directly below the root has
    /// depth 1).
    std::vector<int> dummyDepths() const;

    /// Prints the optimal tree level by level in the same wording as the original program.
    void show_obst(std::ostream& os) const;

private:
    int size;  // number of dummy keys = number of keys + 1
    std::vector<double> p;
    std::vector<double> q;
    std::vector<std::vector<int>> root;
    std::vector<std::vector<double>> e, w;

    void collectDepths(int i, int j, int depth, std::vector<int>& keys,
                       std::vector<int>& dummies) const;
};

}  // namespace introalgo
