// The original vBEstructure.cpp ("not tested, just used to understand the vEB") had no main().
#include <iostream>

#include "introalgo/data_structures/van_emde_boas.hpp"

int main() {
    introalgo::VanEmdeBoas v(16);
    for (int x : {2, 3, 4, 5, 7, 14, 15}) v.insert(x);
    std::cout << "min " << v.minimum() << ", max " << v.maximum() << std::endl;
    std::cout << "successor(5) = " << v.successor(5) << ", predecessor(5) = " << v.predecessor(5)
              << std::endl;
    v.del(7);
    std::cout << "after deleting 7: successor(5) = " << v.successor(5) << std::endl;
}
