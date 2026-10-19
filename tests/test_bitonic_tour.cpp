#include <gtest/gtest.h>

#include <algorithm>
#include <cmath>
#include <limits>
#include <numeric>
#include <set>
#include <vector>

#include "introalgo/dynamic_programming/bitonic_tour.hpp"
#include "test_utils.hpp"

using introalgo::BitonicTour;
using introalgo::Point;

namespace {

double dist(const Point& a, const Point& b) { return std::hypot(a.x - b.x, a.y - b.y); }

double tourLength(const std::vector<Point>& pts, const std::vector<int>& order) {
    double len = 0;
    for (std::size_t i = 0; i < order.size(); ++i)
        len += dist(pts[order[i]], pts[order[(i + 1) % order.size()]]);
    return len;
}

// A closed tour is bitonic when, starting at the leftmost point, x strictly increases up to the
// rightmost point and strictly decreases afterwards.
bool isBitonic(const std::vector<Point>& pts, const std::vector<int>& order) {
    std::size_t peak = 0;
    for (std::size_t i = 0; i < order.size(); ++i)
        if (pts[order[i]].x > pts[order[peak]].x) peak = i;
    for (std::size_t i = 0; i + 1 <= peak; ++i)
        if (!(pts[order[i]].x < pts[order[i + 1]].x)) return false;
    for (std::size_t i = peak; i + 1 < order.size(); ++i)
        if (!(pts[order[i]].x > pts[order[i + 1]].x)) return false;
    return true;
}

// Exhaustive oracle: every permutation that starts at the leftmost point and is bitonic.
double bruteForce(const std::vector<Point>& pts) {
    const std::size_t n = pts.size();
    std::vector<int> idx(n);
    std::iota(idx.begin(), idx.end(), 0);
    std::sort(idx.begin(), idx.end(), [&](int a, int b) { return pts[a].x < pts[b].x; });
    double best = std::numeric_limits<double>::infinity();
    std::vector<int> rest(idx.begin() + 1, idx.end());
    std::sort(rest.begin(), rest.end());  // next_permutation needs the smallest arrangement first
    do {
        std::vector<int> order{idx[0]};
        order.insert(order.end(), rest.begin(), rest.end());
        if (isBitonic(pts, order)) best = std::min(best, tourLength(pts, order));
    } while (std::next_permutation(rest.begin(), rest.end()));
    return best;
}

std::vector<Point> randomPoints(testutil::Rng& rng, int n) {
    std::set<int> xs;
    while (static_cast<int>(xs.size()) < n) xs.insert(testutil::randInt(rng, 0, 200));
    std::vector<int> shuffled(xs.begin(), xs.end());
    std::shuffle(shuffled.begin(), shuffled.end(), rng);  // input is not sorted by x
    std::vector<Point> pts;
    for (int x : shuffled) pts.push_back({double(x), double(testutil::randInt(rng, 0, 200))});
    return pts;
}

}  // namespace

TEST(BitonicTour, TrivialSizes) {
    BitonicTour none({});
    none.solve();
    EXPECT_EQ(none.length(), 0.0);
    EXPECT_TRUE(none.tour().empty());

    BitonicTour one({{3, 4}});
    one.solve();
    EXPECT_EQ(one.length(), 0.0);
    EXPECT_EQ(one.tour(), (std::vector<int>{0}));

    BitonicTour two({{0, 0}, {3, 4}});
    two.solve();
    EXPECT_DOUBLE_EQ(two.length(), 10.0);  // there and back
    EXPECT_EQ(two.tour().size(), 2u);

    BitonicTour three({{0, 0}, {3, 0}, {0.5, 4}});  // sorted by x: (0,0) (0.5,4) (3,0)
    three.solve();
    EXPECT_NEAR(three.length(), 3.0 + std::hypot(0.5, 4) + std::hypot(2.5, 4), 1e-12);
}

TEST(BitonicTour, OriginalDemoPoints) {
    std::vector<Point> pts{{0, 6}, {1, 0}, {2, 3}, {5, 4}, {6, 1}, {7, 5}, {8, 2}};
    BitonicTour t(pts);
    t.solve();
    EXPECT_NEAR(t.length(), 25.584, 1e-3);  // the table value printed by the original program
    EXPECT_NEAR(t.length(), bruteForce(pts), 1e-9);
    EXPECT_NEAR(tourLength(pts, t.tour()), t.length(), 1e-9);
}

TEST(BitonicTour, RejectsEqualXCoordinates) {
    EXPECT_THROW(BitonicTour({{1, 1}, {1, 2}, {3, 3}}), std::invalid_argument);
}

TEST(BitonicTour, RandomizedMatchesExhaustiveSearchAndTourIsValid) {
    testutil::Rng rng(8);
    for (int iter = 0; iter < 300; ++iter) {
        const int n = testutil::randInt(rng, 2, 8);
        auto pts = randomPoints(rng, n);
        BitonicTour t(pts);
        t.solve();
        EXPECT_NEAR(t.length(), bruteForce(pts), 1e-9) << "n=" << n;

        const auto& order = t.tour();
        ASSERT_EQ(order.size(), pts.size());
        std::set<int> visited(order.begin(), order.end());
        EXPECT_EQ(visited.size(), pts.size()) << "every point is visited exactly once";
        EXPECT_TRUE(isBitonic(pts, order));
        EXPECT_NEAR(tourLength(pts, order), t.length(), 1e-9) << "the tour realises the DP value";
    }
}
