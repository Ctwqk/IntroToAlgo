#pragma once

#include <vector>

namespace introalgo {

enum class LpStatus { Optimal, Infeasible, Unbounded };

struct LpResult {
    LpStatus status = LpStatus::Infeasible;
    double objective = 0.0;  ///< value of c.x (only meaningful when status == Optimal)
    std::vector<double> x;   ///< an optimal solution (only meaningful when status == Optimal)
};

/// Simplex algorithm for linear programs in standard form
///
///     maximize   c . x
///     subject to A x <= b,   x >= 0.
///
/// The original SimplexAlgo.cpp is an empty file; this is the textbook (CLRS chapter 29) slack
/// form implementation: pivoting with Bland's smallest-index rule, which cannot cycle, and a
/// first phase with an auxiliary variable x0 that finds a feasible starting point (or proves the
/// program infeasible) when some b_i is negative.
///
/// A pivot costs O((m + n)^2); the number of pivots is exponential in the worst case but small in
/// practice.  Floating point with an absolute tolerance of 1e-9.
class Simplex {
public:
    /// A is m x n, b has m entries and c has n.  Throws std::invalid_argument on shape mismatch.
    Simplex(std::vector<std::vector<double>> A, std::vector<double> b, std::vector<double> c);

    LpResult solve() const;

    static constexpr double kEpsilon = 1e-9;

private:
    int m, n;
    std::vector<std::vector<double>> A;
    std::vector<double> b, c;
};

}  // namespace introalgo
