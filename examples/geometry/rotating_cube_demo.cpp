// Original demo of spinning.cpp (which printed nothing): rotate a 9 x 9 face of the cube and draw it.
#include <iostream>

#include "introalgo/geometry/rotating_cube.hpp"

int main() {
    introalgo::Cube c(9);
    std::cout << "face-on:" << std::endl;
    c.show(std::cout);
    c.getRotateMatd(60, 25, 15);
    c.rotate();
    std::cout << "after rotating by (60, 25, 15) degrees:" << std::endl;
    c.show(std::cout);
}
