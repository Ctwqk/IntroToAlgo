#include "introalgo/math/optimal_stopping.hpp"

#include <stdexcept>

namespace introalgo {

CalFormula::CalFormula(int n) : num(n) {
    if (n < 1) throw std::invalid_argument("CalFormula: need at least one candidate");
}

int CalFormula::getMax() {
    int idx = 0;
    double tmp, max = 0;
    for (int i = 1; i <= num; i++) {
        tmp = calculate(i);
        if (tmp > max) {
            max = tmp;
            idx = i;
        }
    }
    best = max;
    return idx;
}

double CalFormula::calculate(int x) const {
    if (x < 1 || x > num) return 0.0;
    double ans = 0;
    for (int i = 0; i < num - x; i++) ans += 1 / (double)(i + x);
    return ans * (double)x / (double)num;
}

}  // namespace introalgo
