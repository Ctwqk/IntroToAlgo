#include <gtest/gtest.h>

#include <algorithm>
#include <cmath>
#include <limits>
#include <vector>

#include "introalgo/linear_programming/simplex.hpp"
#include "test_utils.hpp"

using introalgo::LpResult;
using introalgo::LpStatus;
using introalgo::Simplex;
using Mat = std::vector<std::vector<double>>;
using Vec = std::vector<double>;

namespace {

// Solves the n x n system M x = r by Gaussian elimination with partial pivoting; false if singular.
bool gauss(Mat M, Vec r, Vec& x) {
    const std::size_t n = M.size();
    for (std::size_t col = 0; col < n; ++col) {
        std::size_t piv = col;
        for (std::size_t i = col + 1; i < n; ++i)
            if (std::fabs(M[i][col]) > std::fabs(M[piv][col])) piv = i;
        if (std::fabs(M[piv][col]) < 1e-10) return false;
        std::swap(M[piv], M[col]);
        std::swap(r[piv], r[col]);
        for (std::size_t i = col + 1; i < n; ++i) {
            const double f = M[i][col] / M[col][col];
            for (std::size_t k = col; k < n; ++k) M[i][k] -= f * M[col][k];
            r[i] -= f * r[col];
        }
    }
    x.assign(n, 0.0);
    for (std::size_t i = n; i-- > 0;) {
        double s = r[i];
        for (std::size_t k = i + 1; k < n; ++k) s -= M[i][k] * x[k];
        x[i] = s / M[i][i];
    }
    return true;
}

struct Oracle {
    bool feasible = false;
    double best = -std::numeric_limits<double>::infinity();
};

// Exhaustive oracle for a *bounded* LP (callers add sum x <= M): the optimum of a pointed polytope
// is attained at a vertex, i.e. where n linearly independent constraints (from A x <= b and
// -x_j <= 0) hold with equality.  Enumerate every choice of n constraints.
Oracle bruteForce(const Mat& A, const Vec& b, const Vec& c) {
    const std::size_t n = c.size(), m = A.size();
    Mat rows = A;
    Vec rhs = b;
    for (std::size_t j = 0; j < n; ++j) {  // x_j >= 0  <=>  -x_j <= 0
        Vec row(n, 0.0);
        row[j] = -1;
        rows.push_back(row);
        rhs.push_back(0.0);
    }
    const std::size_t total = m + n;
    Oracle out;
    // iterate over all n-subsets of the constraints
    std::vector<bool> select(total, false);
    std::fill(select.begin(), select.begin() + static_cast<std::ptrdiff_t>(n), true);
    do {
        Mat M;
        Vec r;
        for (std::size_t i = 0; i < total; ++i)
            if (select[i]) {
                M.push_back(rows[i]);
                r.push_back(rhs[i]);
            }
        Vec x;
        if (!gauss(M, r, x)) continue;
        bool ok = true;
        for (std::size_t i = 0; i < total && ok; ++i) {
            double lhs = 0;
            for (std::size_t j = 0; j < n; ++j) lhs += rows[i][j] * x[j];
            if (lhs > rhs[i] + 1e-7) ok = false;
        }
        if (!ok) continue;
        double obj = 0;
        for (std::size_t j = 0; j < n; ++j) obj += c[j] * x[j];
        out.feasible = true;
        out.best = std::max(out.best, obj);
    } while (std::prev_permutation(select.begin(), select.end()));
    return out;
}

void expectFeasible(const Mat& A, const Vec& b, const Vec& x) {
    for (double xi : x) EXPECT_GE(xi, -1e-7);
    for (std::size_t i = 0; i < A.size(); ++i) {
        double lhs = 0;
        for (std::size_t j = 0; j < x.size(); ++j) lhs += A[i][j] * x[j];
        EXPECT_LE(lhs, b[i] + 1e-6) << "constraint " << i;
    }
}

double dot(const Vec& c, const Vec& x) {
    double s = 0;
    for (std::size_t j = 0; j < c.size(); ++j) s += c[j] * x[j];
    return s;
}

}  // namespace

TEST(Simplex, ClrsExample) {
    // maximize 3x1 + x2 + 2x3 s.t. x1+x2+3x3 <= 30, 2x1+2x2+5x3 <= 24, 4x1+x2+2x3 <= 36
    // optimum 28 at (8, 4, 0)  (CLRS section 29.3).
    Simplex lp({{1, 1, 3}, {2, 2, 5}, {4, 1, 2}}, {30, 24, 36}, {3, 1, 2});
    const LpResult r = lp.solve();
    ASSERT_EQ(r.status, LpStatus::Optimal);
    EXPECT_NEAR(r.objective, 28, 1e-9);
    EXPECT_NEAR(r.x[0], 8, 1e-9);
    EXPECT_NEAR(r.x[1], 4, 1e-9);
    EXPECT_NEAR(r.x[2], 0, 1e-9);
}

TEST(Simplex, TwoVariableTextbookProblem) {
    // maximize 3x + 5y s.t. x <= 4, 2y <= 12, 3x + 2y <= 18: optimum 36 at (2, 6).
    Simplex lp({{1, 0}, {0, 2}, {3, 2}}, {4, 12, 18}, {3, 5});
    const LpResult r = lp.solve();
    ASSERT_EQ(r.status, LpStatus::Optimal);
    EXPECT_NEAR(r.objective, 36, 1e-9);
    EXPECT_NEAR(r.x[0], 2, 1e-9);
    EXPECT_NEAR(r.x[1], 6, 1e-9);
}

TEST(Simplex, NegativeRightHandSideNeedsPhaseOne) {
    // maximize -x - y s.t. -x - y <= -3 (that is x + y >= 3), x <= 5: optimum -3.
    Simplex lp({{-1, -1}, {1, 0}}, {-3, 5}, {-1, -1});
    const LpResult r = lp.solve();
    ASSERT_EQ(r.status, LpStatus::Optimal);
    EXPECT_NEAR(r.objective, -3, 1e-9);
    EXPECT_NEAR(r.x[0] + r.x[1], 3, 1e-9);
}

TEST(Simplex, Infeasible) {
    EXPECT_EQ(Simplex({{1}}, {-1}, {1}).solve().status, LpStatus::Infeasible);  // x <= -1, x >= 0
    EXPECT_EQ(Simplex({{1, 1}, {-1, -1}}, {1, -3}, {1, 1}).solve().status,
              LpStatus::Infeasible);  // x + y <= 1 and x + y >= 3
}

TEST(Simplex, Unbounded) {
    EXPECT_EQ(Simplex({{-1, 1}}, {1}, {1, 0}).solve().status,
              LpStatus::Unbounded);  // x grows with y
    EXPECT_EQ(Simplex({}, {}, {1, 1}).solve().status,
              LpStatus::Unbounded);  // no constraints at all
    // Unbounded *after* phase one:
    EXPECT_EQ(Simplex({{-1, -1}}, {-1}, {1, 1}).solve().status, LpStatus::Unbounded);
}

TEST(Simplex, TrivialAndDegenerateShapes) {
    const LpResult zero = Simplex({}, {}, {}).solve();
    EXPECT_EQ(zero.status, LpStatus::Optimal);
    EXPECT_EQ(zero.objective, 0);
    const LpResult origin =
        Simplex({{1, 1}}, {10}, {-1, -2}).solve();  // maximizing a negative objective
    ASSERT_EQ(origin.status, LpStatus::Optimal);
    EXPECT_NEAR(origin.objective, 0, 1e-12);
    EXPECT_THROW(Simplex({{1, 2}}, {1, 2}, {1, 2}), std::invalid_argument);
    EXPECT_THROW(Simplex({{1}}, {1}, {1, 2}), std::invalid_argument);
}

TEST(Simplex, BealesCyclingExampleTerminates) {
    // Beale's example makes the largest-coefficient rule cycle forever; Bland's rule terminates.
    // maximize 3/4 x1 - 20 x2 + 1/2 x3 - 6 x4  (optimum 5/4 at x1 = 1, x3 = 1)
    Simplex lp({{0.25, -8, -1, 9}, {0.5, -12, -0.5, 3}, {0, 0, 1, 0}}, {0, 0, 1},
               {0.75, -20, 0.5, -6});
    const LpResult r = lp.solve();
    ASSERT_EQ(r.status, LpStatus::Optimal);
    EXPECT_NEAR(r.objective, 1.25, 1e-9);
}

TEST(Simplex, RandomizedMatchesVertexEnumeration) {
    testutil::Rng rng(81);
    int optimal = 0, infeasible = 0;
    for (int iter = 0; iter < 1500; ++iter) {
        const int n = testutil::randInt(rng, 1, 3);
        const int m = testutil::randInt(rng, 1, 4);
        Mat A(static_cast<std::size_t>(m), Vec(static_cast<std::size_t>(n)));
        Vec b(static_cast<std::size_t>(m)), c(static_cast<std::size_t>(n));
        for (auto& row : A)
            for (auto& x : row) x = testutil::randInt(rng, -4, 6);
        for (auto& x : b)
            x = testutil::randInt(rng, -6, 20);  // negative values make some LPs infeasible
        for (auto& x : c) x = testutil::randInt(rng, -5, 8);
        A.push_back(
            Vec(static_cast<std::size_t>(n), 1.0));  // box: sum x <= 15 keeps the problem bounded
        b.push_back(15);

        const Oracle expected = bruteForce(A, b, c);
        const LpResult got = Simplex(A, b, c).solve();
        if (!expected.feasible) {
            EXPECT_EQ(got.status, LpStatus::Infeasible) << "iteration " << iter;
            ++infeasible;
            continue;
        }
        ASSERT_EQ(got.status, LpStatus::Optimal) << "iteration " << iter;
        EXPECT_NEAR(got.objective, expected.best, 1e-6) << "iteration " << iter;
        expectFeasible(A, b, got.x);
        EXPECT_NEAR(dot(c, got.x), got.objective, 1e-6);
        ++optimal;
    }
    EXPECT_GT(optimal, 300);  // the generator really exercises both outcomes
    EXPECT_GT(infeasible, 100);
}

TEST(Simplex, StrongDualityOnRandomPrograms) {
    // The dual of  max c.x, Ax <= b, x >= 0  is  min b.y, A^T y >= c, y >= 0, i.e.
    // max -b.y, -A^T y <= -c, y >= 0.  Both optima must agree (up to sign).
    testutil::Rng rng(82);
    int checked = 0;
    for (int iter = 0; iter < 800; ++iter) {
        const int n = testutil::randInt(rng, 1, 5);
        const int m = testutil::randInt(rng, 1, 5);
        Mat A(static_cast<std::size_t>(m), Vec(static_cast<std::size_t>(n)));
        Vec b(static_cast<std::size_t>(m)), c(static_cast<std::size_t>(n));
        for (auto& row : A)
            for (auto& x : row) x = testutil::randInt(rng, -3, 6);
        for (auto& x : b) x = testutil::randInt(rng, 0, 20);
        for (auto& x : c) x = testutil::randInt(rng, -3, 8);
        const LpResult primal = Simplex(A, b, c).solve();
        if (primal.status != LpStatus::Optimal) continue;

        Mat At(static_cast<std::size_t>(n), Vec(static_cast<std::size_t>(m)));
        for (int i = 0; i < m; ++i)
            for (int j = 0; j < n; ++j) At[j][i] = -A[i][j];
        Vec negC(static_cast<std::size_t>(n)), negB(static_cast<std::size_t>(m));
        for (int j = 0; j < n; ++j) negC[j] = -c[j];
        for (int i = 0; i < m; ++i) negB[i] = -b[i];
        const LpResult dual = Simplex(At, negC, negB).solve();
        ASSERT_EQ(dual.status, LpStatus::Optimal) << "iteration " << iter;
        EXPECT_NEAR(primal.objective, -dual.objective, 1e-6) << "iteration " << iter;
        ++checked;
    }
    EXPECT_GT(checked, 200);
}
