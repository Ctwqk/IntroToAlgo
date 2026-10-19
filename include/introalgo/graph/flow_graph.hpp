#pragma once

#include <map>
#include <stdexcept>
#include <vector>

namespace introalgo {

/// Maximum flow in a directed network with integer capacities (Edmonds-Karp: Ford-Fulkerson with
/// shortest augmenting paths found by BFS), O(V E^2).
///
/// The original flow_graph.cpp was an unfinished template sketch (Node / Edge / Graph /
/// FlowGraph with a capacity graph and a residual graph) that did not compile and contained no
/// flow algorithm.  This keeps its idea -- a capacity graph plus a residual graph, vertices
/// added one at a time -- and completes it.  Every edge is stored together with its reverse
/// (residual) edge, so "residual graph" and "capacity graph" are two views of the same arrays.
class FlowGraph {
public:
    struct EdgeInfo {
        int from, to;
        long long capacity;
        long long flow;
    };

    explicit FlowGraph(int vertices = 0);

    /// Adds a vertex and returns its index (the original addVertice).
    int addVertex();
    int vertices() const { return static_cast<int>(adj.size()); }

    /// Adds a directed edge.  Parallel and antiparallel edges are fine.  Returns false (and adds
    /// nothing) for an unknown vertex, a self-loop or a negative capacity.
    bool addEdge(int x, int y, long long cap);

    /// Computes a maximum s-t flow (any flow from an earlier call is discarded first).
    /// Throws std::invalid_argument if s == t or a vertex is unknown.
    long long maxFlow(int s, int t);

    /// Total flow / capacity / remaining residual capacity on the edges x -> y.
    long long flow(int x, int y) const;
    long long capacity(int x, int y) const;
    long long residual(int x, int y) const;

    /// All original edges with their current flow (for inspection and tests).
    std::vector<EdgeInfo> edges() const;

    /// After maxFlow(s, t): the vertices on the source side of a minimum cut, i.e. those reachable
    /// from s in the residual graph.  The capacity of the edges leaving this set equals the flow.
    std::vector<bool> minCutSourceSide() const;

    /// Sets every flow back to zero.
    void reset();

private:
    struct Arc {
        int to;
        long long cap;   // original capacity (0 for the reverse arc of an edge)
        long long flow;  // flow on the arc; the reverse arc carries the negated value
    };
    std::vector<Arc> arcs;                 // arc 2i is the edge, arc 2i+1 its reverse
    std::vector<std::vector<int>> adj;     // arc indices leaving each vertex
    int source = -1;

    bool valid(int x) const { return x >= 0 && x < vertices(); }
    long long residualOf(const Arc& a) const { return a.cap - a.flow; }
};

/// FlowGraph with arbitrary vertex labels (the original was a template over the vertex value).
template <typename T>
class LabeledFlowGraph {
public:
    /// Returns the index of the vertex with this label, creating it if necessary.
    int addVertex(const T& label) {
        auto it = index.find(label);
        if (it != index.end()) return it->second;
        const int id = graph.addVertex();
        index[label] = id;
        names.push_back(label);
        return id;
    }

    bool addEdge(const T& from, const T& to, long long cap) {
        return graph.addEdge(addVertex(from), addVertex(to), cap);
    }

    long long maxFlow(const T& s, const T& t) { return graph.maxFlow(id(s), id(t)); }
    long long flow(const T& x, const T& y) const { return graph.flow(id(x), id(y)); }
    const FlowGraph& underlying() const { return graph; }
    /// Label of the vertex with this index in underlying().
    const T& label(int id) const { return names.at(static_cast<std::size_t>(id)); }

private:
    FlowGraph graph;
    std::map<T, int> index;
    std::vector<T> names;

    int id(const T& label) const {
        auto it = index.find(label);
        if (it == index.end()) throw std::invalid_argument("LabeledFlowGraph: unknown vertex");
        return it->second;
    }
};

}  // namespace introalgo
