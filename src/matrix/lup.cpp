#include "introalgo/matrix/lup.hpp"

#include <cmath>
#include <ostream>
#include <stdexcept>
#include <utility>

namespace introalgo {

LUP::LUP(Matrix mat) : n(static_cast<int>(mat.size())), A(std::move(mat)), P(static_cast<std::size_t>(n)) {
    for (const auto& row : A)
        if (static_cast<int>(row.size()) != n) throw std::invalid_argument("LUP: matrix must be square");
    double scale = 0;
    for (const auto& row : A)
        for (double x : row) scale = std::fmax(scale, std::fabs(x));
    for (int i = 0; i < n; i++) P[i] = i;
    for (int i = 0; i < n; i++) {
        int max = i;
        for (int k = i + 1; k < n; k++)
            if (std::fabs(A[k][i]) > std::fabs(A[max][i])) max = k;
        if (std::fabs(A[max][i]) <= kSingularTolerance * scale || A[max][i] == 0.0)
            throw std::runtime_error("LUP: matrix is singular");
        if (max != i) {
            std::swap(A[max], A[i]);  // swap the row vectors, no element copying
            std::swap(P[max], P[i]);
            swaps++;
        }
        for (int j = i + 1; j < n; j++) {
            A[j][i] /= A[i][i];
            for (int k = i + 1; k < n; k++) A[j][k] = A[j][k] - A[j][i] * A[i][k];
        }
    }
}

Matrix LUP::L() const {
    Matrix l(static_cast<std::size_t>(n), std::vector<double>(static_cast<std::size_t>(n), 0.0));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < i; j++) l[i][j] = A[i][j];
        l[i][i] = 1;
    }
    return l;
}

Matrix LUP::U() const {
    Matrix u(static_cast<std::size_t>(n), std::vector<double>(static_cast<std::size_t>(n), 0.0));
    for (int i = 0; i < n; i++)
        for (int j = i; j < n; j++) u[i][j] = A[i][j];
    return u;
}

std::vector<double> LUP::solve(const std::vector<double>& rhs) const {
    if (static_cast<int>(rhs.size()) != n) throw std::invalid_argument("LUP::solve: wrong right-hand side size");
    std::vector<double> b(static_cast<std::size_t>(n));
    for (int i = 0; i < n; i++) b[i] = rhs[P[i]];  // apply the permutation
    for (int i = 0; i < n; i++)
        for (int j = 0; j < i; j++) b[i] -= A[i][j] * b[j];  // convert b into y (L y = P b)
    for (int i = n - 1; i >= 0; i--) {                      // convert y into x (U x = y)
        for (int j = i + 1; j < n; j++) b[i] -= b[j] * A[i][j];
        b[i] /= A[i][i];
    }
    return b;
}

double LUP::determinant() const {
    double det = (swaps % 2) ? -1.0 : 1.0;
    for (int i = 0; i < n; i++) det *= A[i][i];
    return det;
}

Matrix LUP::inverse() const {  // the original left getReverse() empty
    Matrix inv(static_cast<std::size_t>(n), std::vector<double>(static_cast<std::size_t>(n), 0.0));
    for (int col = 0; col < n; col++) {
        std::vector<double> e(static_cast<std::size_t>(n), 0.0);
        e[col] = 1;
        const std::vector<double> x = solve(e);
        for (int row = 0; row < n; row++) inv[row][col] = x[row];
    }
    return inv;
}

void LUP::show2(std::ostream& os) const {
    os << 'P' << ":" << std::endl;
    for (int i = 0; i < n; i++) os << P[i] << " ";
    os << std::endl << 'A' << ":" << std::endl;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) os << A[i][j] << " ";
        os << std::endl;
    }
}

Matrix LUP::luNoPivot(Matrix a) {  // LU with space optimization
    const int size = static_cast<int>(a.size());
    for (const auto& row : a)
        if (static_cast<int>(row.size()) != size) throw std::invalid_argument("LUP: matrix must be square");
    for (int i = 0; i < size; i++) {
        if (a[i][i] == 0.0) throw std::runtime_error("LUP::luNoPivot: zero pivot");
        for (int j = i + 1; j < size; j++) {
            a[j][i] /= a[i][i];
            for (int k = i + 1; k < size; k++) a[j][k] = a[j][k] - a[j][i] * a[i][k];
        }
    }
    return a;
}

SolveEquation::SolveEquation(const Matrix& mat, const std::vector<double>& vec)
    : b(LUP(mat).solve(vec)) {}

void SolveEquation::show(std::ostream& os) const {
    for (std::size_t i = 0; i < b.size(); i++) os << b[i] << " ";
}

}  // namespace introalgo
