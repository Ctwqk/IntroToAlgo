#pragma once

#include <iosfwd>
#include <vector>

namespace introalgo {

/// One element of the heap: ordered by `key`, `otherdata` is satellite data.
/// (Called `dat` in the original program.)
struct HeapEntry {
    HeapEntry() = default;
    HeapEntry(int k) : key(k) {}  // NOLINT: implicit on purpose, heap.insert(5) reads well
    HeapEntry(int k, int other) : key(k), otherdata(other) {}

    int getKey() const { return key; }
    int getOtherData() const { return otherdata; }
    bool operator==(const HeapEntry& d) const { return key == d.key; }
    bool operator<(const HeapEntry& d) const { return key < d.key; }
    bool operator>(const HeapEntry& d) const { return key > d.key; }

private:
    int key = 0;
    int otherdata = 0;
};

/// Binary max-heap stored in an array (children of i are 2i+1 and 2i+2).
/// insert / pop / heapify are O(log n), buildHeap is O(n), peek is O(1).
/// The storage grows on demand; `cap` is only the initial capacity.
class Heap {
public:
    explicit Heap(int cap = 100);
    /// Takes the values in the order given and turns them into a heap with buildHeap().
    explicit Heap(const std::vector<int>& input, int cap = 100);

    /// Sifts the element at `pos` down until the heap property holds below it.
    void heapify(int pos);
    /// Same, for the first element with this key.  Returns false if there is none.
    bool heapify(const HeapEntry& d);
    /// Floyd's bottom-up heap construction.
    void buildHeap();

    bool empty() const { return heap.empty(); }
    /// Number of elements (called `sizee` in the original).
    int size() const { return static_cast<int>(heap.size()); }

    void insert(HeapEntry d);
    /// The largest element.  Throws std::out_of_range if the heap is empty.
    HeapEntry peek() const;
    /// Removes the largest element.  Throws std::out_of_range if the heap is empty.
    void pop();

    /// Prints the heap level by level (one line per level, every key followed by a space).
    void showHeap(std::ostream& os) const;

    /// True if every element is >= its children.  Meant for tests.
    bool isMaxHeap() const;

private:
    std::vector<HeapEntry> heap;
    int search(const HeapEntry& d) const;
};

}  // namespace introalgo
