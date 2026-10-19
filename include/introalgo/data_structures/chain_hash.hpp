#pragma once

#include <string>
#include <vector>

namespace introalgo {

struct ListNode {
    int val;
    ListNode* next;
    ListNode() : val(0), next(nullptr) {}
    explicit ListNode(int va) : val(va), next(nullptr) {}
    ListNode(int va, ListNode* n) : val(va), next(n) {}
};

/// Hash table of ints with separate chaining (the original `myHash`).
///
/// It behaves like a multiset: inserting a value twice stores two nodes.  A value lives in the
/// chain of bucket `Hash_func(value)`; `insert` appends at the tail of that chain, `sortedInsert`
/// keeps the chain ordered.  Expected O(1 + load factor) per operation.
class ChainHash {
public:
    /// The original used a fixed table of 10 buckets.
    explicit ChainHash(int buckets = 10);
    ~ChainHash();
    ChainHash(const ChainHash& other);
    ChainHash& operator=(ChainHash other);  // copy-and-swap
    ChainHash(ChainHash&& other) noexcept;

    /// Bucket index of a value, always in [0, bucketCount()) -- also for negative values.
    int Hash_func(int key1) const;

    void insert(int val);
    /// Removes one occurrence; false if the value is not stored.
    bool delete_val(int val);
    /// First node holding the value, or nullptr.
    const ListNode* search(int val) const;
    /// Replaces one occurrence of `tar` by `v` (the new value may belong in another bucket).
    bool modify(int tar, int v);
    /// Inserts into the chain keeping it sorted ("increasing" or "decreasing"), assuming the chain
    /// is already sorted that way.  Throws std::invalid_argument for any other order.
    void sortedInsert(int val, const std::string& ord = "increasing");

    int size() const { return count; }
    int bucketCount() const { return static_cast<int>(map.size()); }
    double loadFactor() const { return static_cast<double>(count) / bucketCount(); }
    /// Values of one chain from head to tail.
    std::vector<int> bucket(int index) const;

private:
    std::vector<ListNode*> map;
    int count = 0;

    void clear();
};

}  // namespace introalgo
