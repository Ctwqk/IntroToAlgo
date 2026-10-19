// Original demo of OBST.cpp (CLRS figure 15.10 style probabilities).
#include <iostream>
#include <vector>

#include "introalgo/dynamic_programming/obst.hpp"

int main() {
    std::vector<double> q{0.06, 0.06, 0.06, 0.06, 0.05, 0.05, 0.05, 0.05};
    std::vector<double> p{0.04, 0.06, 0.08, 0.02, 0.1, 0.12, 0.14};
    introalgo::Obst ob(p, q);
    ob.find_obst2();
    ob.show_obst(std::cout);
}
