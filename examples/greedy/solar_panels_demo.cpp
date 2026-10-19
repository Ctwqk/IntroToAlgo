// The original sy.cpp had a stray main() unrelated to the algorithm; this runs a small instance.
#include <iostream>

#include "introalgo/greedy/solar_panels.hpp"

int main() {
    // Packages: 10 panels (2 installations), 5 panels (1 installation); houses need 4, 6, 9, 11
    // panels.
    introalgo::MaximumNumHousesSolar problem({10, 5}, {2, 1}, {4, 6, 9, 11});
    std::cout << "houses served: " << problem.maxNumHouses() << std::endl;
}
