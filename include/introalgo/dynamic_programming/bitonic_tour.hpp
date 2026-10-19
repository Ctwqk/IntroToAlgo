#pragma once

#include <vector>

namespace introalgo {

struct Point {
    double x = 0.0;
    double y = 0.0;
};

/// Shortest bitonic Euclidean tour (CLRS problem 15-3).
///
/// A bitonic tour starts at the leftmost point, goes strictly rightwards to the
/// rightmost point and comes back strictly leftwards, visiting every point once.
/// The dynamic program b[i][j] (i < j) is the length of the shortest path that
/// starts at p_i, runs left to p_0, turns around and runs right to p_j while
/// covering p_0..p_j.  O(n^2) time and space.
///
/// (The original file was named LongSinglePath and its class `lsp` stopped at
/// "DP tabulated established, cannot re-establish the optimal resolution"; the
/// tour reconstruction is the part that was added.)
class BitonicTour {
public:
    /// The points are sorted by x internally; all x coordinates must be distinct.
    explicit BitonicTour(std::vector<Point> points);

    /// Runs the dynamic program and reconstructs the tour.
    void solve();

    /// Length of the shortest bitonic tour (0 for fewer than two points).
    double length() const { return best; }

    /// Indices into the points passed to the constructor, in tour order, starting at the
    /// leftmost point.  The tour is closed implicitly (last point -> first point).
    const std::vector<int>& tour() const { return order; }

private:
    int size;
    std::vector<Point> pts;                  // sorted by x
    std::vector<int> original;               // original[k]: index of sorted point k in the input
    std::vector<std::vector<double>> cop;    // cop[i][j]: Euclidean distance
    std::vector<std::vector<double>> dptab;  // dptab[i][j] (i < j): b[i][j]
    std::vector<std::vector<int>> prevk;     // prevk[i][j]: predecessor of j in the optimal b[i][j]
    double best = 0.0;
    std::vector<int> order;
};

}  // namespace introalgo
