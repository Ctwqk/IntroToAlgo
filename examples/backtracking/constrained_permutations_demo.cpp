// Original demo of numbers.cpp: permutations of 1..9 with at most 3 increasing and 3 decreasing.
#include <iostream>

#include "introalgo/backtracking/constrained_permutations.hpp"

int main() {
    introalgo::ConstrainedPermutations n(9, 3, 3);
    n.find();
    n.show(std::cout);  // output 100 possible answers
}
