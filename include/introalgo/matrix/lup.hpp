#pragma once

#include <iosfwd>
#include <vector>

namespace introalgo {

using Matrix = std::vector<std::vector<double>>;

/// LUP decomposition with partial pivoting: P A = L U, where P is a permutation, L unit lower
/// triangular and U upper triangular.  The factors are computed in place, in one matrix (the
/// original's `calculate3`, "LUP with space optimization"): L (without its unit diagonal) lives
/// below the diagonal and U on and above it.  O(n^3) to decompose, O(n^2) per solve.
class LUP {
public:
    /// Relative pivot size below which a matrix is considered singular.
    static constexpr double kSingularTolerance = 1e-12;

    /// Decomposes a copy of `mat` (the original overwrote the caller's matrix).
    /// Throws std::invalid_argument if it is not square and std::runtime_error if it is singular.
    explicit LUP(Matrix mat);

    int size() const { return n; }

    /// The packed factors (L below the diagonal, U on and above it).
    const Matrix& packed() const { return A; }
    /// Row i of P A is row permutation()[i] of A.
    const std::vector<int>& permutation() const { return P; }
    Matrix L() const;
    Matrix U() const;

    /// Solves A x = b by forward and back substitution.
    std::vector<double> solve(const std::vector<double>& b) const;
    double determinant() const;
    Matrix inverse() const;

    /// Prints P and the packed matrix like the original show2().
    void show2(std::ostream& os) const;

    /// LU decomposition *without* pivoting (the original's `calculate2`), packed like above.
    /// Throws std::runtime_error on a zero pivot.
    static Matrix luNoPivot(Matrix a);

private:
    int n;
    Matrix A;
    std::vector<int> P;
    int swaps = 0;
};

/// Solves A x = b through an LUP decomposition (the original SolveEquation).
class SolveEquation {
public:
    SolveEquation(const Matrix& mat, const std::vector<double>& vec);
    const std::vector<double>& solution() const { return b; }
    void show(std::ostream& os) const;

private:
    std::vector<double> b;
};

}  // namespace introalgo
