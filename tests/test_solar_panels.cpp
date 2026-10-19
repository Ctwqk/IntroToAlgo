#include <gtest/gtest.h>

#include <algorithm>
#include <vector>

#include "introalgo/graph/flow_graph.hpp"
#include "introalgo/greedy/solar_panels.hpp"
#include "test_utils.hpp"

using introalgo::FlowGraph;
using introalgo::MaximumNumHousesSolar;

namespace {

// Exhaustive oracle: give every house either no package or one of the packages, recursively, and
// keep the best count over all assignments that respect the per-package usage limits.
int bruteForce(const std::vector<int>& panels, std::vector<int> uses, const std::vector<int>& need, std::size_t house = 0) {
    if (house == need.size()) return 0;
    int best = bruteForce(panels, uses, need, house + 1);  // leave this house without panels
    for (std::size_t p = 0; p < panels.size(); ++p) {
        if (panels[p] >= need[house] && uses[p] > 0) {
            --uses[p];
            best = std::max(best, 1 + bruteForce(panels, uses, need, house + 1));
            ++uses[p];
        }
    }
    return best;
}

// Independent oracle: bipartite flow  source -> house (1) -> suitable package -> sink (uses).
int flowOracle(const std::vector<int>& panels, const std::vector<int>& uses, const std::vector<int>& need) {
    const int m = int(need.size()), n = int(panels.size());
    FlowGraph g(m + n + 2);
    const int s = m + n, t = s + 1;
    for (int h = 0; h < m; ++h) {
        g.addEdge(s, h, 1);
        for (int p = 0; p < n; ++p)
            if (panels[std::size_t(p)] >= need[std::size_t(h)]) g.addEdge(h, m + p, 1);
    }
    for (int p = 0; p < n; ++p) g.addEdge(m + p, t, std::max(uses[std::size_t(p)], 0));
    return int(g.maxFlow(s, t));
}

}  // namespace

TEST(SolarPanels, SmallHandCheckedCases) {
    // Packages: 10 panels x2 uses, 5 panels x1 use.  Houses need 4, 6, 9 and 11 panels.
    EXPECT_EQ(MaximumNumHousesSolar({10, 5}, {2, 1}, {4, 6, 9, 11}).maxNumHouses(), 3);
    EXPECT_EQ(MaximumNumHousesSolar({10, 5}, {2, 1}, {11}).maxNumHouses(), 0);
    EXPECT_EQ(MaximumNumHousesSolar({}, {}, {1, 2}).maxNumHouses(), 0);
    EXPECT_EQ(MaximumNumHousesSolar({5}, {3}, {}).maxNumHouses(), 0);
    EXPECT_EQ(MaximumNumHousesSolar({5}, {0}, {1}).maxNumHouses(), 0);  // a package that cannot be used
    // The small package must go to the small house so that the big one still has a package.
    EXPECT_EQ(MaximumNumHousesSolar({3, 8}, {1, 1}, {8, 2}).maxNumHouses(), 2);
}

TEST(SolarPanels, MismatchedPackageArraysAreRejected) {
    EXPECT_THROW(MaximumNumHousesSolar({1, 2}, {1}, {1}), std::invalid_argument);
}

TEST(SolarPanels, RandomizedMatchesExhaustiveAssignmentAndFlow) {
    testutil::Rng rng(91);
    for (int iter = 0; iter < 1000; ++iter) {
        const int n = testutil::randInt(rng, 0, 4), m = testutil::randInt(rng, 0, 6);
        auto panels = testutil::randVector(rng, std::size_t(n), 1, 10);
        auto uses = testutil::randVector(rng, std::size_t(n), 0, 3);
        auto need = testutil::randVector(rng, std::size_t(m), 1, 10);
        const int got = MaximumNumHousesSolar(panels, uses, need).maxNumHouses();
        EXPECT_EQ(got, bruteForce(panels, uses, need)) << "iteration " << iter;
        EXPECT_EQ(got, flowOracle(panels, uses, need)) << "iteration " << iter;
    }
}

TEST(SolarPanels, LargerRandomInstancesAgainstFlow) {
    testutil::Rng rng(92);
    for (int iter = 0; iter < 50; ++iter) {
        const int n = testutil::randInt(rng, 1, 30), m = testutil::randInt(rng, 1, 80);
        auto panels = testutil::randVector(rng, std::size_t(n), 1, 50);
        auto uses = testutil::randVector(rng, std::size_t(n), 0, 5);
        auto need = testutil::randVector(rng, std::size_t(m), 1, 50);
        EXPECT_EQ(MaximumNumHousesSolar(panels, uses, need).maxNumHouses(), flowOracle(panels, uses, need));
    }
}
