#include <gtest/gtest.h>

#include <algorithm>
#include <numeric>
#include <set>
#include <sstream>
#include <vector>

#include "introalgo/graph/graph.hpp"
#include "test_utils.hpp"

using introalgo::Graph;
using introalgo::UFSet;

namespace {

// The 11-vertex undirected 0/1 matrix from the original test_graph().
std::vector<std::vector<int>> originalMatrix() {
    return {{0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0}, {1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0},
            {1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0}, {0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0},
            {1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0}, {0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
            {0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1}, {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0}};
}

// Random graph; `undirected` mirrors every edge.
Graph randomGraph(testutil::Rng& rng, int n, int edges, bool undirected, int maxWeight = 1, bool selfLoops = false) {
    Graph g(n);
    for (int i = 0; i < edges; ++i) {
        const int u = testutil::randInt(rng, 0, n - 1), v = testutil::randInt(rng, 0, n - 1);
        if (u == v && !selfLoops) continue;
        const int w = testutil::randInt(rng, 1, maxWeight);
        if (undirected)
            g.addUndirectedEdge(u, v, w);
        else
            g.addEdge(u, v, w);
    }
    return g;
}

// Oracle for reachability: transitive closure by Warshall's algorithm.
std::vector<std::vector<bool>> closure(const Graph& g) {
    const int n = g.vertices();
    std::vector<std::vector<bool>> reach(static_cast<std::size_t>(n), std::vector<bool>(static_cast<std::size_t>(n), false));
    for (int i = 0; i < n; ++i) {
        reach[i][i] = true;
        for (int j = 0; j < n; ++j)
            if (g.matrix()[i][j]) reach[i][j] = true;
    }
    for (int k = 0; k < n; ++k)
        for (int i = 0; i < n; ++i)
            for (int j = 0; j < n; ++j)
                if (reach[i][k] && reach[k][j]) reach[i][j] = true;
    return reach;
}

// Oracle for directed cycles: some edge u -> v with v reaching u.
bool bruteDirectedCycle(const Graph& g) {
    const auto reach = closure(g);
    for (int u = 0; u < g.vertices(); ++u)
        for (int v = 0; v < g.vertices(); ++v)
            if (g.matrix()[u][v] && reach[v][u]) return true;
    return false;
}

// Oracle for undirected cycles: strip vertices of degree <= 1 until none is left; whatever
// remains contains a cycle.  Self-loops are cycles on their own.
bool bruteUndirectedCycle(const Graph& g) {
    const int n = g.vertices();
    for (int i = 0; i < n; ++i)
        if (g.matrix()[i][i]) return true;
    std::vector<bool> removed(static_cast<std::size_t>(n), false);
    bool progress = true;
    while (progress) {
        progress = false;
        for (int i = 0; i < n; ++i) {
            if (removed[i]) continue;
            int degree = 0;
            for (int j = 0; j < n; ++j)
                if (!removed[j] && g.matrix()[i][j]) ++degree;
            if (degree <= 1) {
                removed[i] = true;
                progress = true;
            }
        }
    }
    return std::count(removed.begin(), removed.end(), false) > 0;
}

bool isTopologicalOrder(const Graph& g, const std::vector<int>& order) {
    if (int(order.size()) != g.vertices()) return false;
    std::vector<int> pos(order.size(), -1);
    for (std::size_t i = 0; i < order.size(); ++i) pos[std::size_t(order[i])] = int(i);
    if (std::count(pos.begin(), pos.end(), -1)) return false;
    for (int u = 0; u < g.vertices(); ++u)
        for (int v : g.adjacency()[u])
            if (pos[u] >= pos[v]) return false;
    return true;
}

}  // namespace

// ---------------------------------------------------------------- UFSet

TEST(UFSet, OriginalDemoPrintsTwentyZeroRanks) {
    UFSet uf(20);
    std::ostringstream out;
    uf.show(out);
    EXPECT_EQ(out.str(), "00000000000000000000");
    EXPECT_EQ(uf.components(), 20);
}

TEST(UFSet, UnionLinksRepresentativesNotArguments) {
    // The original wrote ufset[x] = b, i.e. it re-parented x itself, so merging two non-root
    // elements detached them from their old sets.
    UFSet uf(6);
    uf.Union(0, 1);
    uf.Union(2, 3);
    uf.Union(1, 3);  // neither 1 nor 3 is guaranteed to be a root
    EXPECT_TRUE(uf.connected(0, 2));
    EXPECT_TRUE(uf.connected(0, 3));
    EXPECT_FALSE(uf.connected(0, 4));
    EXPECT_EQ(uf.components(), 3);
    EXPECT_FALSE(uf.Union(0, 3));  // already together
}

TEST(UFSet, AdoptsAParentArray) {
    UFSet uf(std::vector<int>{0, 0, 1, 3, 3});  // sets {0,1,2} and {3,4}
    EXPECT_EQ(uf.components(), 2);
    EXPECT_TRUE(uf.connected(0, 2));
    EXPECT_TRUE(uf.connected(3, 4));
    EXPECT_FALSE(uf.connected(2, 3));
    EXPECT_THROW(UFSet(std::vector<int>{1, 0}), std::invalid_argument);  // cycle
    EXPECT_THROW(UFSet(std::vector<int>{0, 5}), std::invalid_argument);  // out of range
    EXPECT_THROW(uf.Find(5), std::out_of_range);
}

TEST(UFSet, RandomizedMatchesNaiveLabelling) {
    testutil::Rng rng(51);
    for (int iter = 0; iter < 100; ++iter) {
        const int n = testutil::randInt(rng, 1, 40);
        UFSet uf(n);
        std::vector<int> label(static_cast<std::size_t>(n));
        std::iota(label.begin(), label.end(), 0);
        for (int step = 0; step < 60; ++step) {
            const int x = testutil::randInt(rng, 0, n - 1), y = testutil::randInt(rng, 0, n - 1);
            const bool merged = label[x] != label[y];
            const int from = label[y], to = label[x];
            if (merged) std::replace(label.begin(), label.end(), from, to);
            EXPECT_EQ(uf.Union(x, y), merged);
            EXPECT_EQ(uf.components(), int(std::set<int>(label.begin(), label.end()).size()));
        }
        for (int x = 0; x < n; ++x)
            for (int y = 0; y < n; ++y) EXPECT_EQ(uf.connected(x, y), label[x] == label[y]);
    }
}

// ---------------------------------------------------------------- Graph basics

TEST(Graph, MatrixAndAdjacencyConstructorsAgree) {
    Graph fromMatrix(originalMatrix());
    EXPECT_FALSE(fromMatrix.isDirected());
    Graph fromAdj = Graph::fromAdjacency(fromMatrix.adjacency());
    EXPECT_EQ(fromAdj.matrix(), fromMatrix.matrix());
    EXPECT_EQ(fromAdj.adjacency(), fromMatrix.adjacency());
}

TEST(Graph, ConversionsRoundTrip) {
    Graph g(originalMatrix());
    const auto adj = g.adjacency();
    const auto mat = g.matrix();
    g.transMatToAdj();
    EXPECT_EQ(g.adjacency(), adj);
    g.transAdjToMat();
    EXPECT_EQ(g.matrix(), mat);  // 0/1 weights survive the round trip
}

TEST(Graph, ComponentsOfTheOriginalExample) {
    Graph g(originalMatrix());
    EXPECT_EQ(g.componentCount(), 2);  // {0..5} and {6..10}
    EXPECT_TRUE(g.sameComponent(0, 5));
    EXPECT_TRUE(g.sameComponent(6, 10));
    EXPECT_FALSE(g.sameComponent(5, 6));
}

TEST(Graph, ShowFunctionsUseTheRealVertexCount) {
    // showMat() looped to a hard-coded 11 columns in the original.
    Graph g(3);
    g.addUndirectedEdge(0, 1);
    g.addEdge(1, 2, 5);
    std::ostringstream mat, adj;
    g.showMat(mat);
    g.showAdj(adj);
    EXPECT_EQ(mat.str(), "0 1 0\n1 0 5\n0 0 0\n");
    EXPECT_EQ(adj.str(), "1->2--\n2->1--3--\n3->\n");
}

TEST(Graph, BadInputIsRejected) {
    Graph g(3);
    EXPECT_THROW(g.addEdge(0, 3), std::out_of_range);
    EXPECT_THROW(g.addEdge(-1, 0), std::out_of_range);
    EXPECT_THROW(g.addEdge(0, 1, 0), std::invalid_argument);
    EXPECT_THROW(Graph(std::vector<std::vector<int>>{{0, 1}}), std::invalid_argument);
    EXPECT_THROW(g.BFS(7), std::out_of_range);
    EXPECT_THROW(g.DFS(-1), std::out_of_range);
}

TEST(Graph, TraversalOrders) {
    Graph g(6);
    for (auto e : std::vector<std::pair<int, int>>{{0, 1}, {0, 2}, {1, 3}, {2, 3}, {3, 4}}) g.addEdge(e.first, e.second);
    EXPECT_EQ(g.BFS(0), (std::vector<int>{0, 1, 2, 3, 4}));
    EXPECT_EQ(g.DFS(0), (std::vector<int>{0, 1, 3, 4, 2}));
    EXPECT_EQ(g.BFS(5), (std::vector<int>{5}));
}

TEST(Graph, RandomizedTraversalsVisitExactlyTheReachableVertices) {
    testutil::Rng rng(52);
    for (int iter = 0; iter < 200; ++iter) {
        const int n = testutil::randInt(rng, 1, 12);
        Graph g = randomGraph(rng, n, testutil::randInt(rng, 0, 20), testutil::randInt(rng, 0, 1) == 1, 1, true);
        const auto reach = closure(g);
        const int src = testutil::randInt(rng, 0, n - 1);
        for (const auto& order : {g.BFS(src), g.DFS(src)}) {
            EXPECT_EQ(order.front(), src);
            EXPECT_EQ(std::set<int>(order.begin(), order.end()).size(), order.size()) << "no repeats";
            std::set<int> expected;
            for (int v = 0; v < n; ++v)
                if (reach[src][v]) expected.insert(v);
            EXPECT_EQ(std::set<int>(order.begin(), order.end()), expected);
        }
    }
}

TEST(Graph, RandomizedComponentsMatchTransitiveClosureOfTheUndirectedGraph) {
    testutil::Rng rng(53);
    for (int iter = 0; iter < 100; ++iter) {
        const int n = testutil::randInt(rng, 1, 14);
        Graph g = randomGraph(rng, n, testutil::randInt(rng, 0, 14), true);
        const auto reach = closure(g);
        int comps = 0;
        for (int v = 0; v < n; ++v) {
            bool first = true;
            for (int u = 0; u < v; ++u)
                if (reach[u][v]) first = false;
            comps += first ? 1 : 0;
            for (int u = 0; u < n; ++u) EXPECT_EQ(g.sameComponent(u, v), bool(reach[u][v]));
        }
        EXPECT_EQ(g.componentCount(), comps);
    }
}

// ---------------------------------------------------------------- cycles and topological order

TEST(Graph, CycleDetectionKnownCases) {
    Graph chain(3);
    chain.addEdge(0, 1);
    chain.addEdge(1, 2);
    EXPECT_FALSE(chain.isCycle());
    chain.addEdge(2, 0);
    EXPECT_TRUE(chain.isCycle());

    Graph path(3);  // undirected path: the back-and-forth along an edge is not a cycle
    path.addUndirectedEdge(0, 1);
    path.addUndirectedEdge(1, 2);
    EXPECT_FALSE(path.isCycle());
    path.addUndirectedEdge(2, 0);
    EXPECT_TRUE(path.isCycle());

    Graph loop(1);
    loop.addEdge(0, 0);
    EXPECT_TRUE(loop.isCycle());
    EXPECT_FALSE(Graph(0).isCycle());
}

TEST(Graph, RandomizedCycleDetectionMatchesOracles) {
    testutil::Rng rng(54);
    for (int iter = 0; iter < 500; ++iter) {
        const int n = testutil::randInt(rng, 1, 8);
        const bool undirected = testutil::randInt(rng, 0, 1) == 1;
        Graph g = randomGraph(rng, n, testutil::randInt(rng, 0, 9), undirected, 1, true);
        if (g.isDirected())
            EXPECT_EQ(g.isCycle(), bruteDirectedCycle(g));
        else
            EXPECT_EQ(g.isCycle(), bruteUndirectedCycle(g));
    }
}

TEST(Graph, TopologicalSortOnKnownDag) {
    Graph g(6);  // the classic "get dressed" shape
    for (auto e : std::vector<std::pair<int, int>>{{5, 2}, {5, 0}, {4, 0}, {4, 1}, {2, 3}, {3, 1}}) g.addEdge(e.first, e.second);
    ASSERT_TRUE(g.TopoSort().has_value());
    ASSERT_TRUE(g.TopoSortKhan().has_value());
    EXPECT_TRUE(isTopologicalOrder(g, *g.TopoSort()));
    EXPECT_TRUE(isTopologicalOrder(g, *g.TopoSortKhan()));
    g.addEdge(1, 5);  // creates the cycle 5 -> 2 -> 3 -> 1 -> 5
    EXPECT_FALSE(g.TopoSort().has_value());
    EXPECT_FALSE(g.TopoSortKhan().has_value());
}

TEST(Graph, RandomizedTopologicalSortsAreValidExactlyWhenAcyclic) {
    testutil::Rng rng(55);
    for (int iter = 0; iter < 500; ++iter) {
        const int n = testutil::randInt(rng, 1, 9);
        Graph g = randomGraph(rng, n, testutil::randInt(rng, 0, 14), false, 1, true);
        const bool cyclic = bruteDirectedCycle(g);
        const auto dfs = g.TopoSort();
        const auto khan = g.TopoSortKhan();
        EXPECT_EQ(dfs.has_value(), !cyclic);
        EXPECT_EQ(khan.has_value(), !cyclic);
        if (!cyclic) {
            EXPECT_TRUE(isTopologicalOrder(g, *dfs));
            EXPECT_TRUE(isTopologicalOrder(g, *khan));
        }
    }
}

// ---------------------------------------------------------------- minimum spanning forests

namespace {

// Exhaustive oracle: the cheapest acyclic edge subset that has V - (#components) edges.
long long bruteForceMst(const Graph& g) {
    std::vector<introalgo::WeightedEdge> edges;
    for (int i = 0; i < g.vertices(); ++i)
        for (int j = i + 1; j < g.vertices(); ++j)
            if (g.matrix()[i][j]) edges.push_back({i, j, g.matrix()[i][j]});
    Graph copy = g;
    const int need = g.vertices() - copy.componentCount();
    long long best = -1;
    for (unsigned mask = 0; mask < (1u << edges.size()); ++mask) {
        if (__builtin_popcount(mask) != need) continue;
        UFSet uf(g.vertices());
        long long total = 0;
        bool acyclic = true;
        for (std::size_t e = 0; e < edges.size() && acyclic; ++e)
            if (mask >> e & 1u) {
                acyclic = uf.Union(edges[e].u, edges[e].v);
                total += edges[e].weight;
            }
        if (acyclic && (best < 0 || total < best)) best = total;
    }
    return best < 0 ? 0 : best;
}

void expectSpanningForest(const Graph& g, const introalgo::SpanningForest& f) {
    Graph copy = g;
    EXPECT_EQ(int(f.edges.size()), g.vertices() - copy.componentCount());
    UFSet uf(g.vertices());
    long long total = 0;
    for (const auto& e : f.edges) {
        EXPECT_EQ(g.matrix()[e.u][e.v], e.weight);
        EXPECT_TRUE(uf.Union(e.u, e.v)) << "forest edge closes a cycle";
        total += e.weight;
    }
    EXPECT_EQ(total, f.weight);
    for (int u = 0; u < g.vertices(); ++u)
        for (int v = 0; v < g.vertices(); ++v) EXPECT_EQ(uf.connected(u, v), copy.sameComponent(u, v));
}

}  // namespace

TEST(Graph, MinimumSpanningTreeTextbookExample) {
    // CLRS figure 23.1: minimum spanning tree weight 37.
    Graph g(9);
    const int e[][3] = {{0, 1, 4}, {0, 7, 8}, {1, 2, 8}, {1, 7, 11}, {2, 3, 7}, {2, 8, 2}, {2, 5, 4},
                        {3, 4, 9}, {3, 5, 14}, {4, 5, 10}, {5, 6, 2}, {6, 7, 1}, {6, 8, 6}, {7, 8, 7}};
    for (const auto& x : e) g.addUndirectedEdge(x[0], x[1], x[2]);
    EXPECT_EQ(g.MST_K().weight, 37);
    EXPECT_EQ(g.MST_prim().weight, 37);
    expectSpanningForest(g, g.MST_K());
    expectSpanningForest(g, g.MST_prim());
}

TEST(Graph, MinimumSpanningForestOfADisconnectedGraphAndNegativeWeights) {
    Graph g(5);
    g.addUndirectedEdge(0, 1, -3);
    g.addUndirectedEdge(1, 2, 2);
    g.addUndirectedEdge(0, 2, 1);
    g.addUndirectedEdge(3, 4, 7);
    EXPECT_EQ(g.MST_K().weight, -3 + 1 + 7);
    EXPECT_EQ(g.MST_prim().weight, -3 + 1 + 7);
    EXPECT_EQ(Graph(0).MST_K().weight, 0);
}

TEST(Graph, MinimumSpanningTreeRejectsDirectedGraphs) {
    Graph g(2);
    g.addEdge(0, 1);
    EXPECT_THROW(g.MST_K(), std::logic_error);
    EXPECT_THROW(g.MST_prim(), std::logic_error);
}

TEST(Graph, RandomizedMstMatchesExhaustiveSearch) {
    testutil::Rng rng(56);
    for (int iter = 0; iter < 300; ++iter) {
        const int n = testutil::randInt(rng, 1, 6);
        Graph g = randomGraph(rng, n, testutil::randInt(rng, 0, 9), true, 9);
        const long long expected = bruteForceMst(g);
        const auto k = g.MST_K();
        const auto p = g.MST_prim();
        EXPECT_EQ(k.weight, expected);
        EXPECT_EQ(p.weight, expected);
        expectSpanningForest(g, k);
        expectSpanningForest(g, p);
    }
}

// ---------------------------------------------------------------- all-pairs shortest paths (multmat)

TEST(Graph, MultmatOnTheOriginalExampleGivesHopCounts) {
    Graph g(originalMatrix());
    const auto d = g.multmat();
    EXPECT_EQ(d[0][0], 0);
    EXPECT_EQ(d[0][1], 1);
    EXPECT_EQ(d[0][5], 2);  // 0 - 4 - 5
    EXPECT_EQ(d[6][10], 2);
    EXPECT_EQ(d[0][6], Graph::kInfinity);
}

TEST(Graph, MultmatRejectsNegativeWeights) {
    Graph g(2);
    g.addEdge(0, 1, -1);
    EXPECT_THROW(g.multmat(), std::domain_error);
}

TEST(Graph, RandomizedMultmatMatchesFloydWarshall) {
    testutil::Rng rng(57);
    const long long inf = Graph::kInfinity;
    for (int iter = 0; iter < 200; ++iter) {
        const int n = testutil::randInt(rng, 1, 9);
        Graph g = randomGraph(rng, n, testutil::randInt(rng, 0, 25), testutil::randInt(rng, 0, 1) == 1, 9, true);
        std::vector<std::vector<long long>> fw(static_cast<std::size_t>(n), std::vector<long long>(static_cast<std::size_t>(n), inf));
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j)
                if (g.matrix()[i][j]) fw[i][j] = g.matrix()[i][j];
            fw[i][i] = 0;
        }
        for (int k = 0; k < n; ++k)
            for (int i = 0; i < n; ++i)
                for (int j = 0; j < n; ++j)
                    if (fw[i][k] < inf && fw[k][j] < inf) fw[i][j] = std::min(fw[i][j], fw[i][k] + fw[k][j]);
        EXPECT_EQ(g.multmat(), fw);
    }
}
