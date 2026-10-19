#include "introalgo/linear_programming/simplex.hpp"

#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <utility>

namespace introalgo {

namespace {

// Slack form of an LP, as in CLRS: the basic variables B are expressed through the nonbasic ones N,
//     x_i = b_i - sum_{j in N} a_ij x_j     (i in B),     z = v + sum_{j in N} c_j x_j.
// Variables are numbered 0..n-1 (originals), n..n+m-1 (slacks) and n+m (the auxiliary x0).
struct SlackForm {
    std::vector<int> N, B;
    std::vector<std::vector<double>> a;
    std::vector<double> b, c;
    double v = 0.0;

    SlackForm(int vars)
        : a(static_cast<std::size_t>(vars),
            std::vector<double>(static_cast<std::size_t>(vars), 0.0)),
          b(static_cast<std::size_t>(vars), 0.0),
          c(static_cast<std::size_t>(vars), 0.0) {}

    // Exchange the basic variable l with the nonbasic variable e.
    void pivot(int l, int e) {
        const std::size_t nn = a.size();
        std::vector<std::vector<double>> na = a;
        std::vector<double> nb = b, nc = c;
        nb[e] = b[l] / a[l][e];
        for (int j : N)
            if (j != e) na[e][j] = a[l][j] / a[l][e];
        na[e][l] = 1.0 / a[l][e];
        for (int i : B) {
            if (i == l) continue;
            nb[i] = b[i] - a[i][e] * nb[e];
            for (int j : N)
                if (j != e) na[i][j] = a[i][j] - a[i][e] * na[e][j];
            na[i][l] = -a[i][e] * na[e][l];
        }
        const double nv = v + c[e] * nb[e];
        for (int j : N)
            if (j != e) nc[j] = c[j] - c[e] * na[e][j];
        nc[l] = -c[e] * na[e][l];
        // The old row l and column e no longer mean anything; clear them so stale numbers cannot
        // leak.
        for (std::size_t k = 0; k < nn; k++) {
            na[l][k] = 0.0;
            na[k][e] = 0.0;
        }
        nb[l] = 0.0;
        nc[e] = 0.0;
        a = std::move(na);
        b = std::move(nb);
        c = std::move(nc);
        v = nv;
        N.erase(std::find(N.begin(), N.end(), e));
        N.push_back(l);
        B.erase(std::find(B.begin(), B.end(), l));
        B.push_back(e);
    }

    // Runs the simplex method with Bland's rule.  Returns false if the LP is unbounded.
    bool optimize() {
        const double eps = Simplex::kEpsilon;
        while (true) {
            int e = -1;
            for (int j : N)
                if (c[j] > eps && (e == -1 || j < e)) e = j;
            if (e == -1) return true;  // no improving direction: optimal
            int l = -1;
            double best = std::numeric_limits<double>::infinity();
            for (int i : B) {
                if (a[i][e] > eps) {
                    const double ratio = b[i] / a[i][e];
                    if (ratio < best - eps || (std::fabs(ratio - best) <= eps && i < l)) {
                        best = ratio;
                        l = i;
                    }
                }
            }
            if (l == -1) return false;  // the entering variable can grow forever
            pivot(l, e);
        }
    }
};

}  // namespace

Simplex::Simplex(std::vector<std::vector<double>> a, std::vector<double> bb, std::vector<double> cc)
    : m(static_cast<int>(a.size())),
      n(static_cast<int>(cc.size())),
      A(std::move(a)),
      b(std::move(bb)),
      c(std::move(cc)) {
    if (static_cast<int>(b.size()) != m)
        throw std::invalid_argument("Simplex: b must have one entry per row of A");
    for (const auto& row : A)
        if (static_cast<int>(row.size()) != n)
            throw std::invalid_argument("Simplex: every row of A needs n entries");
}

LpResult Simplex::solve() const {
    const double eps = kEpsilon;
    const int x0 = n + m;  // the auxiliary variable of phase one
    SlackForm s(n + m + 1);
    for (int j = 0; j < n; j++) s.N.push_back(j);
    for (int i = 0; i < m; i++) {
        s.B.push_back(n + i);
        s.b[n + i] = b[i];
        for (int j = 0; j < n; j++) s.a[n + i][j] = A[i][j];
    }
    LpResult result;

    // ---- phase one: find a feasible basic solution -----------------------------------------
    int worst = -1;
    for (int i = 0; i < m; i++)
        if (b[i] < -eps && (worst == -1 || b[i] < b[worst])) worst = i;
    if (worst != -1) {
        // Auxiliary LP: maximize -x0 subject to A x - x0 <= b.
        SlackForm aux = s;
        aux.N.push_back(x0);
        for (int i = 0; i < m; i++) aux.a[n + i][x0] = -1.0;
        std::fill(aux.c.begin(), aux.c.end(), 0.0);
        aux.c[x0] = -1.0;
        aux.v = 0.0;
        aux.pivot(n + worst, x0);  // makes every right-hand side non-negative
        aux.optimize();
        if (aux.v < -eps) {
            result.status = LpStatus::Infeasible;
            return result;
        }
        // x0 must leave the basis (it is zero) -- pivot it out if it is still there.
        if (std::find(aux.B.begin(), aux.B.end(), x0) != aux.B.end()) {
            int e = -1;
            for (int j : aux.N)
                if (std::fabs(aux.a[x0][j]) > eps && (e == -1 || j < e)) e = j;
            if (e == -1) {
                // The row says x0 = 0 outright: the variable is redundant, just drop it.
                aux.B.erase(std::find(aux.B.begin(), aux.B.end(), x0));
            } else {
                aux.pivot(x0, e);
            }
        }
        // Drop x0 from the nonbasic set and rebuild the original objective in terms of the
        // current nonbasic variables.
        aux.N.erase(std::remove(aux.N.begin(), aux.N.end(), x0), aux.N.end());
        s = aux;
        std::fill(s.c.begin(), s.c.end(), 0.0);
        s.v = 0.0;
        for (int k = 0; k < n; k++) {
            if (c[k] == 0.0) continue;
            if (std::find(s.N.begin(), s.N.end(), k) != s.N.end()) {
                s.c[k] += c[k];
            } else {
                s.v += c[k] * s.b[k];
                for (int j : s.N) s.c[j] -= c[k] * s.a[k][j];
            }
        }
    } else {
        for (int j = 0; j < n; j++) s.c[j] = c[j];
    }

    // ---- phase two: optimize the real objective ---------------------------------------------
    if (!s.optimize()) {
        result.status = LpStatus::Unbounded;
        return result;
    }
    result.status = LpStatus::Optimal;
    result.objective = s.v;
    result.x.assign(static_cast<std::size_t>(n), 0.0);
    for (int i : s.B)
        if (i < n) result.x[i] = s.b[i];
    return result;
}

}  // namespace introalgo
