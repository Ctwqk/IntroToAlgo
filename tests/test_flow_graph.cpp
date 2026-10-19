#include <gtest/gtest.h>

#include <algorithm>
#include <limits>
#include <string>
#include <vector>

#include "introalgo/graph/flow_graph.hpp"
#include "test_utils.hpp"

using introalgo::FlowGraph;
using introalgo::LabeledFlowGraph;

namespace {

struct Edge {
    int from, to;
    long long cap;
};

FlowGraph build(int n, const std::vector<Edge>& edges) {
    FlowGraph g(n);
    for (const auto& e : edges) g.addEdge(e.from, e.to, e.cap);
    return g;
}

// Exhaustive minimum cut: every subset S containing s but not t, capacity of the edges leaving S.
long long bruteForceMinCut(int n, const std::vector<Edge>& edges, int s, int t) {
    long long best = -1;
    for (unsigned mask = 0; mask < (1u << n); ++mask) {
        if (!(mask >> s & 1u) || (mask >> t & 1u)) continue;
        long long cut = 0;
        for (const auto& e : edges)
            if ((mask >> e.from & 1u) && !(mask >> e.to & 1u)) cut += e.cap;
        if (best < 0 || cut < best) best = cut;
    }
    return best;
}

// A flow is feasible if it respects capacities and conserves flow at every inner vertex; its value
// is the net flow leaving the source.
void expectValidFlow(const FlowGraph& g, int s, int t, long long value) {
    std::vector<long long> net(std::size_t(g.vertices()), 0);  // inflow - outflow
    for (const auto& e : g.edges()) {
        EXPECT_GE(e.flow, 0);
        EXPECT_LE(e.flow, e.capacity);
        net[std::size_t(e.to)] += e.flow;
        net[std::size_t(e.from)] -= e.flow;
    }
    for (int v = 0; v < g.vertices(); ++v) {
        if (v == s) {
            EXPECT_EQ(-net[std::size_t(v)], value);
        } else if (v == t) {
            EXPECT_EQ(net[std::size_t(v)], value);
        } else {
            EXPECT_EQ(net[std::size_t(v)], 0) << "flow is not conserved at vertex " << v;
        }
    }
}

}  // namespace

TEST(FlowGraph, TextbookNetwork) {
    // CLRS figure 26.1: s = 0, t = 5, maximum flow 23.
    FlowGraph g = build(6, {{0, 1, 16},
                            {0, 2, 13},
                            {1, 2, 10},
                            {2, 1, 4},
                            {1, 3, 12},
                            {3, 2, 9},
                            {2, 4, 14},
                            {4, 3, 7},
                            {3, 5, 20},
                            {4, 5, 4}});
    EXPECT_EQ(g.maxFlow(0, 5), 23);
    expectValidFlow(g, 0, 5, 23);
    EXPECT_EQ(g.maxFlow(0, 5), 23) << "calling it again discards the old flow";
}

TEST(FlowGraph, SmallCasesAndEdgeConditions) {
    FlowGraph chain = build(3, {{0, 1, 5}, {1, 2, 3}});
    EXPECT_EQ(chain.maxFlow(0, 2), 3);
    EXPECT_EQ(chain.flow(0, 1), 3);
    EXPECT_EQ(chain.capacity(0, 1), 5);
    EXPECT_EQ(chain.residual(0, 1), 2);
    EXPECT_EQ(chain.residual(1, 0), 3);  // can cancel the flow that was sent

    FlowGraph disconnected = build(4, {{0, 1, 5}, {2, 3, 5}});
    EXPECT_EQ(disconnected.maxFlow(0, 3), 0);

    FlowGraph parallel = build(2, {{0, 1, 2}, {0, 1, 3}});
    EXPECT_EQ(parallel.maxFlow(0, 1), 5);
    EXPECT_EQ(parallel.capacity(0, 1), 5);

    FlowGraph opposite = build(2, {{0, 1, 4}, {1, 0, 9}});
    EXPECT_EQ(opposite.maxFlow(0, 1), 4);

    FlowGraph zero = build(2, {{0, 1, 0}});
    EXPECT_EQ(zero.maxFlow(0, 1), 0);
}

TEST(FlowGraph, InvalidInput) {
    FlowGraph g(2);
    EXPECT_FALSE(g.addEdge(0, 2, 1));   // unknown vertex
    EXPECT_FALSE(g.addEdge(0, 0, 1));   // self-loop
    EXPECT_FALSE(g.addEdge(0, 1, -1));  // negative capacity
    EXPECT_TRUE(g.edges().empty());
    EXPECT_THROW(g.maxFlow(0, 0), std::invalid_argument);
    EXPECT_THROW(g.maxFlow(0, 5), std::invalid_argument);
    EXPECT_EQ(g.addVertex(), 2);
    EXPECT_EQ(g.vertices(), 3);
}

TEST(FlowGraph, MinCutSourceSideCapacityEqualsTheFlow) {
    FlowGraph g = build(6, {{0, 1, 16},
                            {0, 2, 13},
                            {1, 2, 10},
                            {2, 1, 4},
                            {1, 3, 12},
                            {3, 2, 9},
                            {2, 4, 14},
                            {4, 3, 7},
                            {3, 5, 20},
                            {4, 5, 4}});
    const long long value = g.maxFlow(0, 5);
    const auto side = g.minCutSourceSide();
    EXPECT_TRUE(side[0]);
    EXPECT_FALSE(side[5]);
    long long cut = 0;
    for (const auto& e : g.edges())
        if (side[std::size_t(e.from)] && !side[std::size_t(e.to)]) cut += e.capacity;
    EXPECT_EQ(cut, value);
}

TEST(FlowGraph, RandomizedMaxFlowEqualsMinCutAndIsAValidFlow) {
    testutil::Rng rng(61);
    for (int iter = 0; iter < 600; ++iter) {
        const int n = testutil::randInt(rng, 2, 8);
        std::vector<Edge> edges;
        const int m = testutil::randInt(rng, 0, 18);
        for (int i = 0; i < m; ++i) {
            const int u = testutil::randInt(rng, 0, n - 1), v = testutil::randInt(rng, 0, n - 1);
            if (u != v) edges.push_back({u, v, testutil::randInt(rng, 0, 12)});
        }
        const int s = 0, t = n - 1;
        FlowGraph g = build(n, edges);
        const long long value = g.maxFlow(s, t);
        EXPECT_EQ(value, bruteForceMinCut(n, edges, s, t)) << "iteration " << iter;
        expectValidFlow(g, s, t, value);

        // The min cut extracted from the residual graph is a minimum cut.
        const auto side = g.minCutSourceSide();
        long long cut = 0;
        for (const auto& e : edges)
            if (side[std::size_t(e.from)] && !side[std::size_t(e.to)]) cut += e.cap;
        EXPECT_EQ(cut, value);
    }
}

TEST(FlowGraph, BipartiteMatchingViaUnitCapacities) {
    // 3 left x 3 right, edges chosen so that the maximum matching has size 2 (Hall violation:
    // left vertices 0 and 1 both only see right vertex 3).
    FlowGraph g(8);  // 0 = source, 1..3 left, 4..6 right, 7 = sink
    for (int l = 1; l <= 3; ++l) g.addEdge(0, l, 1);
    for (int r = 4; r <= 6; ++r) g.addEdge(r, 7, 1);
    g.addEdge(1, 4, 1);
    g.addEdge(2, 4, 1);
    g.addEdge(3, 5, 1);
    g.addEdge(3, 6, 1);
    EXPECT_EQ(g.maxFlow(0, 7), 2);
}

TEST(FlowGraph, LargeNetworkRunsInReasonableTime) {
    // A layered network with 2000 vertices; the flow value is known: every layer has capacity 1
    // per vertex, so the flow equals the layer width.
    const int layers = 100, width = 20;
    FlowGraph g(layers * width + 2);
    const int s = layers * width, t = s + 1;
    for (int w = 0; w < width; ++w) {
        g.addEdge(s, w, 1);
        g.addEdge((layers - 1) * width + w, t, 1);
    }
    for (int l = 0; l + 1 < layers; ++l)
        for (int w = 0; w < width; ++w) g.addEdge(l * width + w, (l + 1) * width + w, 1);
    EXPECT_EQ(g.maxFlow(s, t), width);
}

TEST(LabeledFlowGraph, VerticesCanBeAnyOrderedType) {
    LabeledFlowGraph<std::string> g;
    g.addEdge("source", "a", 10);
    g.addEdge("source", "b", 5);
    g.addEdge("a", "b", 15);
    g.addEdge("a", "sink", 5);
    g.addEdge("b", "sink", 10);
    EXPECT_EQ(g.maxFlow("source", "sink"), 15);
    EXPECT_EQ(g.flow("a", "sink"), 5);
    EXPECT_EQ(g.underlying().vertices(), 4);
    EXPECT_THROW(g.maxFlow("source", "nowhere"), std::invalid_argument);
}

TEST(FlowGraph, IntegerCapacityLimitsAreChecked) {
    const long long hi = std::numeric_limits<long long>::max();
    FlowGraph single = build(2, {{0, 1, hi}});
    EXPECT_EQ(single.maxFlow(0, 1), hi);
    EXPECT_EQ(single.residual(1, 0), hi);
    FlowGraph parallel = build(2, {{0, 1, hi}, {0, 1, 1}});
    EXPECT_THROW(parallel.capacity(0, 1), std::overflow_error);
    EXPECT_THROW(parallel.residual(0, 1), std::overflow_error);
    EXPECT_THROW(parallel.maxFlow(0, 1), std::overflow_error);
    // A failed solve discards the partial flow; the graph can be solved again.
    EXPECT_EQ(parallel.flow(0, 1), 0);
    EXPECT_EQ(parallel.maxFlow(1, 0), 0);
    FlowGraph opposite = build(2, {{0, 1, hi}, {1, 0, hi}});
    EXPECT_EQ(opposite.maxFlow(0, 1), hi);
    EXPECT_THROW(opposite.residual(1, 0), std::overflow_error);
}
