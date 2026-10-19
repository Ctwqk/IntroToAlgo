// The original SimplexAlgo.cpp was an empty file; this solves the CLRS section 29.3 example:
//   maximize 3x1 + x2 + 2x3  s.t.  x1 + x2 + 3x3 <= 30,  2x1 + 2x2 + 5x3 <= 24,  4x1 + x2 + 2x3 <= 36.
#include <iostream>

#include "introalgo/linear_programming/simplex.hpp"

int main() {
    introalgo::Simplex lp({{1, 1, 3}, {2, 2, 5}, {4, 1, 2}}, {30, 24, 36}, {3, 1, 2});
    const introalgo::LpResult r = lp.solve();
    if (r.status != introalgo::LpStatus::Optimal) {
        std::cout << "no optimum" << std::endl;
        return 1;
    }
    std::cout << "optimal objective: " << r.objective << std::endl;
    std::cout << "x = (" << r.x[0] << ", " << r.x[1] << ", " << r.x[2] << ")" << std::endl;
}
