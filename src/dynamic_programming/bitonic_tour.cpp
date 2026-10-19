#include "introalgo/dynamic_programming/bitonic_tour.hpp"

#include <algorithm>
#include <cmath>
#include <numeric>
#include <stdexcept>
#include <utility>

namespace introalgo {

BitonicTour::BitonicTour(std::vector<Point> points) : size(static_cast<int>(points.size())) {
    original.resize(points.size());
    std::iota(original.begin(), original.end(), 0);
    std::sort(original.begin(), original.end(),
              [&](int a, int b) { return points[a].x < points[b].x; });
    for (int k = 1; k < size; k++)
        if (points[original[k]].x == points[original[k - 1]].x)
            throw std::invalid_argument("BitonicTour: x coordinates must be distinct");
    pts.reserve(points.size());
    for (int idx : original) pts.push_back(points[idx]);

    const auto n = static_cast<std::size_t>(size);
    cop.assign(n, std::vector<double>(n, 0.0));
    dptab.assign(n, std::vector<double>(n, 0.0));
    prevk.assign(n, std::vector<int>(n, -1));
    for (std::size_t i = 0; i < n; i++)
        for (std::size_t j = 0; j < n; j++)
            cop[i][j] = std::hypot(pts[i].x - pts[j].x, pts[i].y - pts[j].y);
}

void BitonicTour::solve() {
    best = 0.0;
    order.clear();
    if (size == 0) return;
    if (size == 1) {
        order.push_back(0);
        return;
    }
    for (int i = 0; i < size - 1; i++) {
        // b[i][i+1]: the point i+1 hangs off some k < i (or i == 0).
        int min = 0;
        for (int k = 1; k < i; k++) {
            if (dptab[min][i] + cop[min][i + 1] > dptab[k][i] + cop[k][i + 1]) min = k;
        }
        dptab[i][i + 1] = dptab[min][i] + cop[min][i + 1];
        prevk[i][i + 1] = min;
        // b[i][j] for j > i+1: j simply follows j-1 on the same chain.
        for (int j = i + 2; j < size; j++) {
            dptab[i][j] = dptab[i][j - 1] + cop[j - 1][j];
            prevk[i][j] = j - 1;
        }
    }
    best = dptab[size - 2][size - 1] + cop[size - 2][size - 1];

    // Reconstruction: walk the two chains back to p_0.  x ends at i, y ends at j.
    std::vector<int> x{size - 2}, y{size - 1};
    int i = size - 2, j = size - 1;
    while (!(i == 0 && j == 1)) {
        if (j > i + 1) {
            y.push_back(j - 1);
            j--;
        } else {
            const int k = prevk[i][j];
            y.push_back(k);
            std::swap(x, y);  // the chain that ended at j-1 becomes the right-hand one
            j = i;
            i = k;
        }
    }
    y.push_back(0);
    order.reserve(static_cast<std::size_t>(size));
    for (auto it = x.rbegin(); it != x.rend(); ++it) order.push_back(original[*it]);
    for (std::size_t k = 0; k + 1 < y.size(); k++) order.push_back(original[y[k]]);
}

}  // namespace introalgo
