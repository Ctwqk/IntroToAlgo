#include <gtest/gtest.h>

#include <algorithm>
#include <cmath>
#include <sstream>
#include <vector>

#include "introalgo/matrix/lup.hpp"
#include "test_utils.hpp"

using introalgo::LUP;
using introalgo::Matrix;
using introalgo::SolveEquation;

namespace {

Matrix randomMatrix(testutil::Rng& rng, int n, double lo = -10, double hi = 10) {
    Matrix m(static_cast<std::size_t>(n), std::vector<double>(static_cast<std::size_t>(n)));
    for (auto& row : m)
        for (auto& x : row) x = testutil::randReal(rng, lo, hi);
    return m;
}

Matrix multiply(const Matrix& a, const Matrix& b) {
    const std::size_t n = a.size();
    Matrix c(n, std::vector<double>(n, 0.0));
    for (std::size_t i = 0; i < n; ++i)
        for (std::size_t j = 0; j < n; ++j)
            for (std::size_t k = 0; k < n; ++k) c[i][j] += a[i][k] * b[k][j];
    return c;
}

double maxAbsDiff(const Matrix& a, const Matrix& b) {
    double d = 0;
    for (std::size_t i = 0; i < a.size(); ++i)
        for (std::size_t j = 0; j < a.size(); ++j) d = std::max(d, std::fabs(a[i][j] - b[i][j]));
    return d;
}

// Oracle for the determinant: Laplace expansion along the first row.
double cofactorDeterminant(const Matrix& m) {
    const std::size_t n = m.size();
    if (n == 0) return 1;
    if (n == 1) return m[0][0];
    double det = 0;
    for (std::size_t c = 0; c < n; ++c) {
        Matrix minor;
        for (std::size_t r = 1; r < n; ++r) {
            std::vector<double> row;
            for (std::size_t k = 0; k < n; ++k)
                if (k != c) row.push_back(m[r][k]);
            minor.push_back(row);
        }
        det += (c % 2 ? -1 : 1) * m[0][c] * cofactorDeterminant(minor);
    }
    return det;
}

}  // namespace

TEST(Lup, SolveEquationFromOriginalDemo) {
    // The answer given in the original comment: -3/19, -1/19, 59/19.
    SolveEquation se({{1, 5, 4}, {2, 0, 3}, {5, 8, 2}}, {12, 9, 5});
    EXPECT_NEAR(se.solution()[0], -3.0 / 19, 1e-12);
    EXPECT_NEAR(se.solution()[1], -1.0 / 19, 1e-12);
    EXPECT_NEAR(se.solution()[2], 59.0 / 19, 1e-12);
    std::ostringstream out;
    se.show(out);
    EXPECT_EQ(out.str().substr(0, 9), "-0.157895");
}

TEST(Lup, DecompositionOfTheOriginalTestMatrix) {
    const Matrix a{{2, 0, 2, 0.6}, {3, 3, 4, -2}, {5, 5, 4, 2}, {-1, -2, 3.4, -1}};
    LUP lup(a);
    EXPECT_EQ(lup.permutation()[0], 2);  // the largest entry of the first column (5) is pivoted to the top
    Matrix pa;
    for (int row : lup.permutation()) pa.push_back(a[std::size_t(row)]);
    EXPECT_LT(maxAbsDiff(multiply(lup.L(), lup.U()), pa), 1e-12);
    std::ostringstream out;
    lup.show2(out);
    EXPECT_EQ(out.str().substr(0, 4), "P:\n2");
}

TEST(Lup, InputMatrixIsNotModified) {
    // The original decomposed the caller's matrix in place (and swapped its row pointers).
    const Matrix a{{0, 1}, {1, 0}};
    Matrix copy = a;
    LUP lup(copy);
    EXPECT_EQ(copy, a);
}

TEST(Lup, PivotingHandlesAZeroOnTheDiagonal) {
    LUP lup({{0, 1}, {1, 0}});
    const auto x = lup.solve({3, 4});
    EXPECT_DOUBLE_EQ(x[0], 4);
    EXPECT_DOUBLE_EQ(x[1], 3);
    EXPECT_DOUBLE_EQ(lup.determinant(), -1);
    EXPECT_THROW(LUP::luNoPivot({{0, 1}, {1, 0}}), std::runtime_error);
}

TEST(Lup, SingularAndMalformedMatricesAreRejected) {
    EXPECT_THROW(LUP({{1, 2}, {2, 4}}), std::runtime_error);
    EXPECT_THROW(LUP({{0, 0}, {0, 0}}), std::runtime_error);
    EXPECT_THROW(LUP({{1, 2, 3}, {4, 5, 6}}), std::invalid_argument);
    LUP ok({{2, 0}, {0, 2}});
    EXPECT_THROW(ok.solve({1, 2, 3}), std::invalid_argument);
    EXPECT_DOUBLE_EQ(LUP(Matrix{}).determinant(), 1.0);
}

TEST(Lup, RandomizedFactorisationSolveDeterminantAndInverse) {
    testutil::Rng rng(71);
    for (int iter = 0; iter < 300; ++iter) {
        const int n = testutil::randInt(rng, 1, 7);
        const Matrix a = randomMatrix(rng, n);
        LUP lup(a);

        // P A = L U
        Matrix pa;
        for (int row : lup.permutation()) pa.push_back(a[std::size_t(row)]);
        EXPECT_LT(maxAbsDiff(multiply(lup.L(), lup.U()), pa), 1e-9);
        // P is a permutation
        auto perm = lup.permutation();
        std::sort(perm.begin(), perm.end());
        for (int i = 0; i < n; ++i) EXPECT_EQ(perm[std::size_t(i)], i);
        // partial pivoting keeps every multiplier in L at most 1 in absolute value
        const Matrix l = lup.L();
        for (int i = 0; i < n; ++i)
            for (int j = 0; j < i; ++j) EXPECT_LE(std::fabs(l[i][j]), 1.0 + 1e-12);

        // A x = b
        std::vector<double> b(static_cast<std::size_t>(n));
        for (auto& x : b) x = testutil::randReal(rng, -5, 5);
        const auto x = lup.solve(b);
        for (int i = 0; i < n; ++i) {
            double lhs = 0;
            for (int j = 0; j < n; ++j) lhs += a[i][j] * x[j];
            EXPECT_NEAR(lhs, b[i], 1e-7);
        }

        // det and inverse
        if (n <= 5) {
            const double det = cofactorDeterminant(a);
            EXPECT_NEAR(lup.determinant(), det, 1e-7 * std::max(1.0, std::fabs(det)));
        }
        Matrix identity(static_cast<std::size_t>(n), std::vector<double>(static_cast<std::size_t>(n), 0.0));
        for (int i = 0; i < n; ++i) identity[i][i] = 1;
        EXPECT_LT(maxAbsDiff(multiply(a, lup.inverse()), identity), 1e-6);
    }
}

TEST(Lup, NoPivotFactorisationOnDiagonallyDominantMatrices) {
    testutil::Rng rng(72);
    for (int iter = 0; iter < 100; ++iter) {
        const int n = testutil::randInt(rng, 1, 6);
        Matrix a = randomMatrix(rng, n, -1, 1);
        for (int i = 0; i < n; ++i) a[i][i] += 2.0 * n;  // strictly diagonally dominant: no zero pivots
        const Matrix packed = LUP::luNoPivot(a);
        Matrix l(static_cast<std::size_t>(n), std::vector<double>(static_cast<std::size_t>(n), 0.0)), u = l;
        for (int i = 0; i < n; ++i)
            for (int j = 0; j < n; ++j) {
                if (j < i) l[i][j] = packed[i][j];
                else u[i][j] = packed[i][j];
                if (i == j) l[i][j] = 1;
            }
        EXPECT_LT(maxAbsDiff(multiply(l, u), a), 1e-9);
    }
}
