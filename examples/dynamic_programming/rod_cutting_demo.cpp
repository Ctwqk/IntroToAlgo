// The original rogcutting.cpp was an empty class; this runs the CLRS price table.
#include <iostream>

#include "introalgo/dynamic_programming/rod_cutting.hpp"

int main() {
    introalgo::RodCutting rod({1, 5, 8, 9, 10, 17, 17, 20, 24, 30});
    for (int n : {4, 7, 10}) {
        std::cout << "rod of length " << n << ": revenue " << rod.bestRevenue(n) << ", pieces";
        for (int piece : rod.cuts(n)) std::cout << " " << piece;
        std::cout << std::endl;
    }
}
