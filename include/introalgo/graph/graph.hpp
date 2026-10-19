#pragma once

#include <iosfwd>
#include <optional>
#include <vector>

namespace introalgo {

/// Disjoint-set forest (union by rank, with path compression).
class UFSet {
public:
    /// n singleton sets 0..n-1.
    explicit UFSet(int n);
    /// Adopts a parent array: element i hangs below parent[i] (roots are their own parent).
    /// Throws std::invalid_argument if an entry is out of range or the array contains a cycle.
    explicit UFSet(const std::vector<int>& parent);

    /// Representative of the set containing x.
    int Find(int x);
    /// Merges the sets of x and y.  Returns false if they were already one set.
    bool Union(int x, int y);
    bool connected(int x, int y) { return Find(x) == Find(y); }

    int size() const { return static_cast<int>(ufset.size()); }
    /// Number of disjoint sets.
    int components() const { return sets; }
    /// Prints the rank of every element (what the original show() did).
    void show(std::ostream& os) const;

private:
    std::vector<int> ufset;
    std::vector<int> rank;
    int sets;
};

struct WeightedEdge {
    int u, v;
    int weight;
};

struct SpanningForest {
    long long weight = 0;
    std::vector<WeightedEdge> edges;
};

/// Graph on vertices 0..V-1 kept both as adjacency lists and as a weight matrix; a non-zero
/// entry mat[i][j] is an edge i -> j with that weight (so zero-weight edges cannot be
/// represented).  An undirected graph is one whose matrix is symmetric (isDirected() == false).
///
/// Self-loops are allowed; parallel edges are not (adding an edge again replaces its weight).
class Graph {
public:
    explicit Graph(int v);
    /// From a V x V weight matrix (the original graph(int** m, int v)).
    explicit Graph(const std::vector<std::vector<int>>& m);
    /// From adjacency lists (the original graph(vector<int>* a, int v)); every edge gets weight 1.
    static Graph fromAdjacency(const std::vector<std::vector<int>>& a);

    int vertices() const { return V; }
    const std::vector<std::vector<int>>& adjacency() const { return adj; }
    const std::vector<std::vector<int>>& matrix() const { return mat; }

    /// Adds (or re-weights) the directed edge u -> v.  Throws std::out_of_range for a bad vertex
    /// and std::invalid_argument for weight 0.
    void addEdge(int u, int v, int weight = 1);
    /// Adds u -> v and v -> u.
    void addUndirectedEdge(int u, int v, int weight = 1);

    /// Prints "1->2--3--" style lines (vertices are 1-based in the output, as in the original).
    void showAdj(std::ostream& os) const;
    /// Prints the matrix, one row per line.
    void showMat(std::ostream& os) const;

    bool isDirected() const;
    /// Rebuilds the adjacency lists from the matrix / the matrix from the adjacency lists.
    void transMatToAdj();
    void transAdjToMat();
    /// Rebuilds the union-find structure from the edges (connectivity ignoring direction).
    void getUFset();
    /// Number of connected components (weakly connected for directed graphs).
    int componentCount();
    bool sameComponent(int u, int v);

    /// Vertices reachable from src, in depth-first preorder (neighbours in adjacency order).
    std::vector<int> DFS(int src = 0) const;
    /// Vertices reachable from src, in breadth-first order.
    std::vector<int> BFS(int src = 0) const;

    /// Directed graph: is there a directed cycle?  Undirected graph: is there a cycle (a
    /// back-and-forth along one edge does not count)?  A self-loop is a cycle in both.
    bool isCycle() const;

    /// A topological order (every edge goes from an earlier to a later vertex), or nullopt if
    /// the graph has a cycle.  TopoSort is the depth-first version (reverse postorder),
    /// TopoSortKhan is Kahn's algorithm.
    std::optional<std::vector<int>> TopoSort() const;
    std::optional<std::vector<int>> TopoSortKhan() const;

    /// Minimum spanning forest of an undirected graph (a tree per component).  Throws
    /// std::logic_error for a directed graph.  Prim: O(E log V); Kruskal: O(E log E).
    SpanningForest MST_prim() const;
    SpanningForest MST_K() const;

    static constexpr long long kInfinity = 1LL << 60;
    /// All-pairs shortest path lengths by repeated min-plus squaring of the weight matrix
    /// (O(V^3 log V)); kInfinity marks unreachable pairs.  Weights must be positive, otherwise
    /// std::domain_error is thrown.
    std::vector<std::vector<long long>> multmat() const;

private:
    int V;
    std::vector<std::vector<int>> adj;
    std::vector<std::vector<int>> mat;
    UFSet uf;
};

}  // namespace introalgo
