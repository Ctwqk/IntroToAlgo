// Original demo of max_heap.cpp: insert 21 values and print the heap level by level.
#include <iostream>
#include <vector>

#include "introalgo/data_structures/max_heap.hpp"

int main() {
    std::vector<int> values{10,    33,  44,   1323, 42,  99,  0,   1,    4, 29, 34,
                            63546, 478, 4452, 464,  245, 243, 345, 2345, 4, 5};
    introalgo::Heap h;
    for (int i : values) h.insert(introalgo::HeapEntry(i));
    h.showHeap(std::cout);
    std::cout << "largest: " << h.peek().getKey() << std::endl;
}
