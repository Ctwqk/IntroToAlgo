// Original demo of MaxQuadExp.cpp: the expression can be given on the command line.
#include <iostream>
#include <string>

#include "introalgo/dynamic_programming/max_quad_exp.hpp"

int main(int argc, char** argv) {
    std::string exp = "5-8+7*4-8+9";
    if (argc > 1) exp = argv[1];
    try {
        introalgo::Expression e(exp);
        long long best = e.calculateMax();
        e.display(std::cout);
        std::cout << best << std::endl;
        std::cout << e.maxExpression() << " = " << best << std::endl;
    } catch (const std::exception& ex) {
        std::cerr << "error: " << ex.what() << std::endl;
        return 1;
    }
}
