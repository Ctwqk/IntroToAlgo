#include "introalgo/data_structures/max_heap.hpp"

#include <algorithm>
#include <ostream>
#include <stdexcept>
#include <utility>

namespace introalgo {

Heap::Heap(int cap) {
    heap.reserve(static_cast<std::size_t>(std::max(cap, 0)));
}

Heap::Heap(const std::vector<int>& input, int cap)
    : Heap(std::max(cap, static_cast<int>(input.size()))) {
    for (int i : input) heap.push_back(i);
    buildHeap();
}

void Heap::pop() {
    if (heap.empty()) throw std::out_of_range("Heap::pop: empty heap");
    heap.front() = heap.back();
    heap.pop_back();
    heapify(0);
}

HeapEntry Heap::peek() const {
    if (heap.empty()) throw std::out_of_range("Heap::peek: empty heap");
    return heap.front();
}

int Heap::search(const HeapEntry& d) const {
    for (int i = 0; i < size(); i++) {
        if (heap[i] == d) return i;
    }
    return -1;
}

bool Heap::heapify(const HeapEntry& d) {
    const int pos = search(d);
    if (pos == -1) return false;
    heapify(pos);
    return true;
}

void Heap::heapify(int pos) {
    if (pos < 0 || pos >= size()) return;  // out of bound
    const int n = size();
    int q;
    while (2 * pos + 1 < n) {
        if (2 * pos + 2 >= n)
            q = 2 * pos + 1;
        else
            q = heap[2 * pos + 1] > heap[2 * pos + 2] ? 2 * pos + 1 : 2 * pos + 2;
        if (heap[q] > heap[pos]) {
            std::swap(heap[q], heap[pos]);
            pos = q;
        } else {
            break;
        }
    }
}

void Heap::buildHeap() {
    for (int i = size() / 2 - 1; i >= 0; i--) {
        heapify(i);
    }
}

void Heap::insert(HeapEntry d) {
    heap.push_back(d);
    int pos = size() - 1;
    while (pos) {
        // The parent of position pos is (pos - 1) / 2.  The original used pos / 2, which is
        // the parent in a 1-based heap and disagreed with the 0-based children used by heapify.
        const int parent = (pos - 1) / 2;
        if (heap[pos] > heap[parent]) {
            std::swap(heap[pos], heap[parent]);
            pos = parent;
        } else {
            break;
        }
    }
}

void Heap::showHeap(std::ostream& os) const {
    const int n = size();
    for (int first = 0, width = 1; first < n; first += width, width *= 2) {
        for (int k = 0; k < width && first + k < n; k++) os << heap[first + k].getKey() << " ";
        os << std::endl;
    }
}

bool Heap::isMaxHeap() const {
    for (int i = 1; i < size(); i++)
        if (heap[(i - 1) / 2] < heap[i]) return false;
    return true;
}

}  // namespace introalgo
