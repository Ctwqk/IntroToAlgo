#include "introalgo/graph/graph.hpp"

#include <algorithm>
#include <functional>
#include <ostream>
#include <queue>
#include <stdexcept>
#include <tuple>

namespace introalgo {

/*below is the union-find set*/
UFSet::UFSet(int n) : ufset(static_cast<std::size_t>(n < 0 ? 0 : n)), rank(ufset.size(), 0), sets(size()) {
    for (int i = 0; i < size(); i++) ufset[i] = i;
}

UFSet::UFSet(const std::vector<int>& arr) : ufset(arr), rank(arr.size(), 0), sets(0) {
    const int n = size();
    for (int i = 0; i < n; i++)
        if (ufset[i] < 0 || ufset[i] >= n) throw std::invalid_argument("UFSet: parent out of range");
    for (int i = 0; i < n; i++) {
        int x = i, steps = 0;
        while (ufset[x] != x) {
            x = ufset[x];
            if (++steps > n) throw std::invalid_argument("UFSet: parent array contains a cycle");
        }
        if (ufset[i] == i) sets++;
    }
}

void UFSet::show(std::ostream& os) const {
    for (int i = 0; i < size(); i++) os << rank[i];
}

int UFSet::Find(int x) {
    if (x < 0 || x >= size()) throw std::out_of_range("UFSet::Find");
    int root = x;
    while (ufset[root] != root) root = ufset[root];
    while (ufset[x] != root) {  // path compression
        const int next = ufset[x];
        ufset[x] = root;
        x = next;
    }
    return root;
}

bool UFSet::Union(int x, int y) {
    // Work on the representatives.  (The original linked x and y themselves, which only
    // works if both happen to be roots.)
    int a = Find(x);
    int b = Find(y);
    if (a == b) return false;
    if (rank[a] < rank[b]) std::swap(a, b);
    ufset[b] = a;
    if (rank[a] == rank[b]) rank[a]++;
    sets--;
    return true;
}

/*below is the graph*/
Graph::Graph(int v)
    : V(v),
      adj(static_cast<std::size_t>(v < 0 ? 0 : v)),
      mat(adj.size(), std::vector<int>(adj.size(), 0)),
      uf(v) {
    if (v < 0) throw std::invalid_argument("Graph: negative vertex count");
}

Graph::Graph(const std::vector<std::vector<int>>& m) : Graph(static_cast<int>(m.size())) {
    for (const auto& row : m)
        if (row.size() != m.size()) throw std::invalid_argument("Graph: matrix must be square");
    mat = m;
    transMatToAdj();
    getUFset();
}

Graph Graph::fromAdjacency(const std::vector<std::vector<int>>& a) {
    Graph g(static_cast<int>(a.size()));
    for (int i = 0; i < g.V; i++)
        for (int j : a[i]) g.addEdge(i, j);
    g.getUFset();
    return g;
}

void Graph::addEdge(int u, int v, int weight) {
    if (u < 0 || u >= V || v < 0 || v >= V) throw std::out_of_range("Graph::addEdge: bad vertex");
    if (weight == 0) throw std::invalid_argument("Graph::addEdge: weight 0 means 'no edge'");
    if (mat[u][v] == 0) adj[u].push_back(v);
    mat[u][v] = weight;
    uf.Union(u, v);
}

void Graph::addUndirectedEdge(int u, int v, int weight) {
    addEdge(u, v, weight);
    addEdge(v, u, weight);
}

void Graph::showAdj(std::ostream& os) const {
    for (int i = 0; i < V; i++) {
        os << i + 1 << "->";
        for (std::vector<int>::const_iterator j = adj[i].begin(); j != adj[i].end(); j++) os << *j + 1 << "--";
        os << std::endl;
    }
}

void Graph::showMat(std::ostream& os) const {
    for (int i = 0; i < V; i++) {
        for (int j = 0; j < V; j++) os << mat[i][j] << (j + 1 < V ? " " : "");
        os << std::endl;
    }
}

void Graph::transMatToAdj() {
    for (auto& list : adj) list.clear();
    for (int i = 0; i < V; i++) {
        for (int j = 0; j < V; j++) {
            if (mat[i][j]) adj[i].push_back(j);
        }
    }
}

void Graph::transAdjToMat() {
    for (auto& row : mat) std::fill(row.begin(), row.end(), 0);
    for (int i = 0; i < V; i++) {
        for (std::size_t j = 0; j < adj[i].size(); j++) {
            mat[i][adj[i][j]] = 1;
        }
    }
}

bool Graph::isDirected() const {
    for (int i = 0; i < V; i++) {
        for (int j = i + 1; j < V; j++) {
            if (mat[i][j] != mat[j][i]) return true;
        }
    }
    return false;
}

void Graph::getUFset() {
    uf = UFSet(V);
    for (int i = 0; i < V; i++) {
        for (std::size_t j = 0; j < adj[i].size(); j++) {
            uf.Union(i, adj[i][j]);
        }
    }
}

int Graph::componentCount() { return uf.components(); }

bool Graph::sameComponent(int u, int v) { return uf.connected(u, v); }

std::vector<int> Graph::DFS(int src) const {
    if (src < 0 || src >= V) throw std::out_of_range("Graph::DFS: bad vertex");
    std::vector<int> order;
    std::vector<bool> seen(static_cast<std::size_t>(V), false);
    // Explicit stack of (vertex, index of the next neighbour to look at).
    std::vector<std::pair<int, std::size_t>> stack{{src, 0}};
    seen[src] = true;
    order.push_back(src);
    while (!stack.empty()) {
        auto& top = stack.back();
        if (top.second == adj[top.first].size()) {
            stack.pop_back();
            continue;
        }
        const int next = adj[top.first][top.second++];
        if (!seen[next]) {
            seen[next] = true;
            order.push_back(next);
            stack.push_back({next, 0});
        }
    }
    return order;
}

std::vector<int> Graph::BFS(int src) const {
    if (src < 0 || src >= V) throw std::out_of_range("Graph::BFS: bad vertex");
    std::vector<int> order;
    std::vector<bool> seen(static_cast<std::size_t>(V), false);
    std::queue<int> q;
    q.push(src);
    seen[src] = true;
    while (!q.empty()) {
        const int u = q.front();
        q.pop();
        order.push_back(u);
        for (int w : adj[u]) {
            if (!seen[w]) {
                seen[w] = true;
                q.push(w);
            }
        }
    }
    return order;
}

bool Graph::isCycle() const {
    for (int i = 0; i < V; i++)
        if (mat[i][i]) return true;
    if (!isDirected()) {
        // Undirected: a cycle exists exactly when some edge joins two vertices that are
        // already connected.
        UFSet sets(V);
        for (int i = 0; i < V; i++)
            for (int j = i + 1; j < V; j++)
                if (mat[i][j] && !sets.Union(i, j)) return true;
        return false;
    }
    // Directed: depth-first search looking for a back edge.
    enum { White, Grey, Black };
    std::vector<int> colour(static_cast<std::size_t>(V), White);
    for (int s = 0; s < V; s++) {
        if (colour[s] != White) continue;
        std::vector<std::pair<int, std::size_t>> stack{{s, 0}};
        colour[s] = Grey;
        while (!stack.empty()) {
            auto& top = stack.back();
            if (top.second == adj[top.first].size()) {
                colour[top.first] = Black;
                stack.pop_back();
                continue;
            }
            const int next = adj[top.first][top.second++];
            if (colour[next] == Grey) return true;
            if (colour[next] == White) {
                colour[next] = Grey;
                stack.push_back({next, 0});
            }
        }
    }
    return false;
}

std::optional<std::vector<int>> Graph::TopoSort() const {
    enum { White, Grey, Black };
    std::vector<int> colour(static_cast<std::size_t>(V), White);
    std::vector<int> postorder;
    for (int s = 0; s < V; s++) {
        if (colour[s] != White) continue;
        std::vector<std::pair<int, std::size_t>> stack{{s, 0}};
        colour[s] = Grey;
        while (!stack.empty()) {
            auto& top = stack.back();
            if (top.second == adj[top.first].size()) {
                colour[top.first] = Black;
                postorder.push_back(top.first);
                stack.pop_back();
                continue;
            }
            const int next = adj[top.first][top.second++];
            if (colour[next] == Grey) return std::nullopt;  // back edge: cycle
            if (colour[next] == White) {
                colour[next] = Grey;
                stack.push_back({next, 0});
            }
        }
    }
    std::reverse(postorder.begin(), postorder.end());
    return postorder;
}

std::optional<std::vector<int>> Graph::TopoSortKhan() const {
    std::vector<int> indegree(static_cast<std::size_t>(V), 0);
    for (int i = 0; i < V; i++)
        for (int j : adj[i]) indegree[j]++;
    std::queue<int> ready;
    for (int i = 0; i < V; i++)
        if (indegree[i] == 0) ready.push(i);
    std::vector<int> order;
    while (!ready.empty()) {
        const int u = ready.front();
        ready.pop();
        order.push_back(u);
        for (int w : adj[u])
            if (--indegree[w] == 0) ready.push(w);
    }
    if (static_cast<int>(order.size()) != V) return std::nullopt;
    return order;
}

SpanningForest Graph::MST_prim() const {
    if (isDirected()) throw std::logic_error("Graph::MST_prim: the graph is directed");
    SpanningForest forest;
    std::vector<bool> inTree(static_cast<std::size_t>(V), false);
    using Entry = std::tuple<int, int, int>;  // (weight, to, from)
    for (int start = 0; start < V; start++) {
        if (inTree[start]) continue;
        std::priority_queue<Entry, std::vector<Entry>, std::greater<Entry>> frontier;
        frontier.push({0, start, -1});
        while (!frontier.empty()) {
            auto [w, to, from] = frontier.top();
            frontier.pop();
            if (inTree[to]) continue;
            inTree[to] = true;
            if (from != -1) {
                forest.weight += w;
                forest.edges.push_back({from, to, w});
            }
            for (int next : adj[to])
                if (next != to && !inTree[next]) frontier.push({mat[to][next], next, to});
        }
    }
    return forest;
}

SpanningForest Graph::MST_K() const {
    if (isDirected()) throw std::logic_error("Graph::MST_K: the graph is directed");
    std::vector<WeightedEdge> edges;
    for (int i = 0; i < V; i++)
        for (int j = i + 1; j < V; j++)
            if (mat[i][j]) edges.push_back({i, j, mat[i][j]});
    std::sort(edges.begin(), edges.end(),
              [](const WeightedEdge& a, const WeightedEdge& b) { return a.weight < b.weight; });
    SpanningForest forest;
    UFSet sets(V);
    for (const auto& e : edges) {
        if (sets.Union(e.u, e.v)) {
            forest.weight += e.weight;
            forest.edges.push_back(e);
        }
    }
    return forest;
}

std::vector<std::vector<long long>> Graph::multmat() const {
    for (int i = 0; i < V; i++)
        for (int j = 0; j < V; j++)
            if (mat[i][j] < 0) throw std::domain_error("Graph::multmat: negative weight");
    std::vector<std::vector<long long>> d(static_cast<std::size_t>(V),
                                          std::vector<long long>(static_cast<std::size_t>(V), kInfinity));
    for (int i = 0; i < V; i++) {
        for (int j = 0; j < V; j++)
            if (mat[i][j]) d[i][j] = mat[i][j];
        d[i][i] = 0;
    }
    // d holds shortest paths using at most 2^r edges after r rounds; stop when nothing changes.
    bool changed = true;
    while (changed) {
        changed = false;
        auto next = d;
        for (int i = 0; i < V; i++)
            for (int j = 0; j < V; j++)
                for (int k = 0; k < V; k++)
                    if (d[i][k] < kInfinity && d[k][j] < kInfinity && d[i][k] + d[k][j] < next[i][j]) {
                        next[i][j] = d[i][k] + d[k][j];
                        changed = true;
                    }
        d = std::move(next);
    }
    return d;
}

}  // namespace introalgo
