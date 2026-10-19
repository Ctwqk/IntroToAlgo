// Original demo of beststop.cpp: the secretary problem with 200 candidates.
#include <iostream>

#include "introalgo/math/optimal_stopping.hpp"

int main() {
    introalgo::CalFormula c(200);
    const int reject = c.getMax();
    std::cout << c.bestProbability() << std::endl;
    std::cout << reject << std::endl;
}
