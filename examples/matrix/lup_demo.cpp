// Original demos of LUP.cpp: testSolveEquation (the answer is -3/19, -1/19, 59/19) and testLUP.
#include <iostream>

#include "introalgo/matrix/lup.hpp"

int main() {
    introalgo::SolveEquation se({{1, 5, 4}, {2, 0, 3}, {5, 8, 2}}, {12, 9, 5});
    se.show(std::cout);
    std::cout << std::endl;

    introalgo::LUP lup({{2, 0, 2, 0.6}, {3, 3, 4, -2}, {5, 5, 4, 2}, {-1, -2, 3.4, -1}});
    lup.show2(std::cout);
    std::cout << "determinant: " << lup.determinant() << std::endl;
}
