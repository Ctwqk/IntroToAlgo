#include "introalgo/dynamic_programming/max_quad_exp.hpp"

#include <iomanip>
#include <ostream>
#include <stdexcept>
#include <utility>

namespace introalgo {

namespace {

bool isOperator(char c) {
    return c == '+' || c == '-' || c == '*' || c == 'x' || c == '/';
}

}  // namespace

Expression::Expression(std::string e) : exp(std::move(e)) {
    // Split "n0 op0 n1 op1 ... nk" into nums / opes.
    long long tmp = 0;
    int digits = 0;
    for (char c : exp) {
        if (c == ' ') continue;
        if (c >= '0' && c <= '9') {
            if (++digits > 15) throw std::invalid_argument("Expression: number too long");
            tmp = tmp * 10 + (c - '0');
        } else if (isOperator(c)) {
            if (digits == 0)
                throw std::invalid_argument("Expression: operator without a left operand");
            nums.push_back(tmp);
            opes.push_back(c);
            tmp = 0;
            digits = 0;
        } else {
            throw std::invalid_argument(std::string("Expression: unexpected character '") + c +
                                        "'");
        }
    }
    if (digits == 0) throw std::invalid_argument("Expression: empty or ends with an operator");
    nums.push_back(tmp);
    num = static_cast<int>(nums.size());

    const auto n = static_cast<std::size_t>(num);
    dp.assign(n, std::vector<long long>(n, 0));
    valid.assign(n, std::vector<bool>(n, false));
    path.assign(n, std::vector<Choice>(n));
    for (int i = 0; i < num; i++) {
        dp[i][i] = nums[i];
        valid[i][i] = true;
    }
}

namespace {

// Returns false when the operation is undefined (division by zero).
bool apply(long long a, long long b, char c, long long& out) {
    switch (c) {
        case '+':
            out = a + b;
            return true;
        case '-':
            out = a - b;
            return true;
        case '*':
        case 'x':
            out = a * b;
            return true;
        case '/':
            if (b == 0) return false;
            out = a / b;
            return true;
    }
    return false;
}

}  // namespace

void Expression::solve() {
    if (solved) return;
    for (int i = 1; i < num; i++) {
        for (int j = 0; j < num - i; j++) {
            const int r = j + i;  // dp[j][r]: maximum, dp[r][j]: minimum of operands j..r
            for (int k = j; k < r; k++) {
                // Any combination of the extremes of the two halves can produce the extreme of
                // the whole, depending on the operator and on the signs involved: try all four.
                for (int c = 0; c < 4; c++) {
                    const bool lm = (c & 1) != 0, rm = (c & 2) != 0;
                    const int la = lm ? j : k, lb = lm ? k : j;  // table cell of the left part
                    const int ra = rm ? k + 1 : r, rb = rm ? r : k + 1;
                    if (!valid[la][lb] || !valid[ra][rb]) continue;
                    long long v;
                    if (!apply(dp[la][lb], dp[ra][rb], opes[k], v)) continue;
                    Choice choice{k, lm, rm};
                    if (!valid[j][r] || v > dp[j][r]) {
                        dp[j][r] = v;
                        valid[j][r] = true;
                        path[j][r] = choice;
                    }
                    if (!valid[r][j] || v < dp[r][j]) {
                        dp[r][j] = v;
                        valid[r][j] = true;
                        path[r][j] = choice;
                    }
                }
            }
        }
    }
    solved = true;
}

long long Expression::calculateMax() {
    solve();
    if (!valid[0][num - 1])
        throw std::domain_error("Expression: every parenthesization divides by zero");
    return dp[0][num - 1];
}

long long Expression::calculateMin() {
    solve();
    if (!valid[num - 1][0])
        throw std::domain_error("Expression: every parenthesization divides by zero");
    return dp[num - 1][0];
}

std::string Expression::build(int l, int r, bool wantMax) const {
    if (l == r) return std::to_string(nums[l]);
    const Choice& c = wantMax ? path[l][r] : path[r][l];
    return "(" + build(l, c.split, c.leftMax) + std::string(1, opes[c.split]) +
           build(c.split + 1, r, c.rightMax) + ")";
}

std::string Expression::maxExpression() {
    calculateMax();
    return build(0, num - 1, true);
}

void Expression::display(std::ostream& os) const {
    for (int i = 0; i < num; i++) {
        for (int j = 0; j < num; j++) os << std::setw(6) << dp[i][j] << " ";
        os << std::endl;
    }
}

}  // namespace introalgo
