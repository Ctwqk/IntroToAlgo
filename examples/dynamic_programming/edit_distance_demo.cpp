// The original editDistance.cpp had no main(); this runs the textbook example.
#include <iostream>

#include "introalgo/dynamic_programming/edit_distance.hpp"

int main() {
    introalgo::ED ed("kitten", "sitting");
    std::cout << "edit distance(kitten, sitting) = " << ed.calculate() << std::endl;
}
