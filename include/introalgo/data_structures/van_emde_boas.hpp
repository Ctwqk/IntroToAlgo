#pragma once

#include <memory>
#include <vector>

namespace introalgo {

/// van Emde Boas tree (CLRS chapter 20) over the universe {0, ..., u-1}.
///
/// The structure is the recursive one of the original `vBE` class: every node keeps its minimum
/// and maximum (the minimum is *not* stored in a cluster), a summary structure over the
/// non-empty clusters and sqrt(u) clusters.  member / insert / del / successor / predecessor are
/// O(log log u).  Compared with the original, the universe is rounded up to a power of two, the
/// split into clusters is done with shifts (the original used log2 in the index arithmetic and
/// never allocated its clusters) and clusters are created on first use, so the memory is
/// proportional to the number of stored keys times log log u rather than to u.
///
/// "No element" is reported as -1, as in the original.
class VanEmdeBoas {
public:
    /// `universe` is rounded up to the next power of two (and to at least 2); throws
    /// std::invalid_argument for values above 2^30.
    explicit VanEmdeBoas(int universe);

    /// Size of the universe after rounding.
    int universe() const { return u; }

    int minimum() const { return min; }  // -1 when empty
    int maximum() const { return max; }  // -1 when empty
    bool empty() const { return min == -1; }

    bool member(int x) const;
    /// Returns false (and does nothing) if x is outside the universe or already present.
    bool insert(int x);
    /// Returns false if x is not stored.
    bool del(int x);
    /// Smallest stored element > x, or -1.
    int successor(int x) const;
    /// Largest stored element < x, or -1 (the original called this `precessor`).
    int predecessor(int x) const;

private:
    int u;
    int lowBits;                      // clusters hold 2^lowBits elements
    int min = -1, max = -1;
    std::unique_ptr<VanEmdeBoas> summary;
    std::vector<std::unique_ptr<VanEmdeBoas>> cluster;

    int high(int x) const { return x >> lowBits; }
    int low(int x) const { return x & ((1 << lowBits) - 1); }
    int idx(int h, int l) const { return (h << lowBits) | l; }
    VanEmdeBoas& clusterAt(int h);
    bool emptyInsert(int x);
    bool insertImpl(int x);
    bool delImpl(int x);
};

}  // namespace introalgo
