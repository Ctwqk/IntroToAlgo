#pragma once

#include <iosfwd>
#include <string>
#include <vector>

namespace introalgo {

/// Maximum value of an arithmetic expression over all ways to parenthesize it.
///
/// The expression is a list of non-negative integers joined by the binary
/// operators + - * / (and `x` as an alias of `*`), e.g. "5-8+7*4-8+9".  Because
/// the product of two negative sub-results can be large, the dynamic program
/// keeps both the maximum and the minimum value of every sub-expression.  Both
/// live in one table as in the original program: dp[i][j] with i <= j is the
/// maximum of operands i..j and dp[j][i] is the minimum.
///
/// + - * are exact.  Integer division '/' is evaluated on the extreme
/// sub-results only, which is a heuristic (the quotient is not monotone in the
/// divisor); it always returns a value that some parenthesization really
/// produces, but not necessarily the largest one.  Parenthesizations that divide
/// by zero are skipped.  Values are 64-bit and overflow is not checked.
///
/// O(n^3) time and O(n^2) space for n operands.
class Expression {
public:
    /// Throws std::invalid_argument for a malformed expression.
    explicit Expression(std::string exp);

    /// Largest value over all parenthesizations.  Throws std::domain_error if every
    /// parenthesization divides by zero.
    long long calculateMax();

    /// Smallest value over all parenthesizations.
    long long calculateMin();

    /// A fully parenthesized expression that evaluates to calculateMax().
    std::string maxExpression();

    /// Prints the dp table (maxima above the diagonal, minima below).
    void display(std::ostream& os) const;

    int operandCount() const { return num; }

private:
    struct Choice {
        int split = -1;       // operator index k: (l..k) op (k+1..r)
        bool leftMax = true;  // use the maximum (true) or minimum of the left part
        bool rightMax = true;
    };

    std::string exp;
    std::vector<long long> nums;
    std::vector<char> opes;
    std::vector<std::vector<long long>> dp;
    std::vector<std::vector<bool>> valid;
    std::vector<std::vector<Choice>> path;
    int num = 0;
    bool solved = false;

    void solve();
    std::string build(int l, int r, bool wantMax) const;
};

}  // namespace introalgo
