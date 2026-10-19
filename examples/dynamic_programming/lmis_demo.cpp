// The original LMIS.cpp had no main(); this runs a small example.
#include <iostream>

#include "introalgo/dynamic_programming/lmis.hpp"

int main() {
    introalgo::Lmis l({10, 9, 2, 5, 3, 7, 101, 18});
    std::cout << "longest increasing subsequence has length " << l.findLMIS() << ": ";
    for (int v : l.subsequence()) std::cout << v << " ";
    std::cout << std::endl;
}
