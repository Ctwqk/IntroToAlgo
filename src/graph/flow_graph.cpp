#include "introalgo/graph/flow_graph.hpp"

#include <algorithm>
#include <limits>
#include <queue>

namespace introalgo {

FlowGraph::FlowGraph(int v) : adj(static_cast<std::size_t>(v < 0 ? 0 : v)) {}

int FlowGraph::addVertex() {
    adj.emplace_back();
    return vertices() - 1;
}

bool FlowGraph::addEdge(int x, int y, long long cap) {
    if (!valid(x) || !valid(y) || x == y || cap < 0) return false;
    adj[x].push_back(static_cast<int>(arcs.size()));
    arcs.push_back({y, cap, 0});
    adj[y].push_back(static_cast<int>(arcs.size()));
    arcs.push_back({x, 0, 0});
    return true;
}

void FlowGraph::reset() {
    for (auto& a : arcs) a.flow = 0;
}

long long FlowGraph::maxFlow(int s, int t) {
    if (!valid(s) || !valid(t) || s == t)
        throw std::invalid_argument("FlowGraph::maxFlow: bad source/sink");
    reset();
    source = s;
    long long total = 0;
    std::vector<int> parentArc(adj.size());
    while (true) {
        // BFS for a shortest augmenting path in the residual graph.
        std::fill(parentArc.begin(), parentArc.end(), -1);
        std::queue<int> q;
        q.push(s);
        parentArc[s] = -2;
        while (!q.empty() && parentArc[t] == -1) {
            const int u = q.front();
            q.pop();
            for (int id : adj[u]) {
                const Arc& a = arcs[id];
                if (parentArc[a.to] == -1 && residualOf(a) > 0) {
                    parentArc[a.to] = id;
                    q.push(a.to);
                }
            }
        }
        if (parentArc[t] == -1) break;  // no augmenting path: the flow is maximum

        long long bottleneck = std::numeric_limits<long long>::max();
        for (int v = t; v != s; v = arcs[parentArc[v] ^ 1].to)
            bottleneck = std::min(bottleneck, residualOf(arcs[parentArc[v]]));
        for (int v = t; v != s; v = arcs[parentArc[v] ^ 1].to) {
            arcs[parentArc[v]].flow += bottleneck;
            arcs[parentArc[v] ^ 1].flow -= bottleneck;
        }
        total += bottleneck;
    }
    return total;
}

long long FlowGraph::flow(int x, int y) const {
    long long f = 0;
    if (!valid(x) || !valid(y)) return 0;
    for (int id : adj[x])
        if (id % 2 == 0 && arcs[id].to == y) f += arcs[id].flow;
    return f;
}

long long FlowGraph::capacity(int x, int y) const {
    long long c = 0;
    if (!valid(x) || !valid(y)) return 0;
    for (int id : adj[x])
        if (id % 2 == 0 && arcs[id].to == y) c += arcs[id].cap;
    return c;
}

long long FlowGraph::residual(int x, int y) const {
    // Forward edges x -> y still have cap - flow to give; edges y -> x can be cancelled.
    long long r = 0;
    if (!valid(x) || !valid(y)) return 0;
    for (int id : adj[x])
        if (arcs[id].to == y) r += residualOf(arcs[id]);
    return r;
}

std::vector<FlowGraph::EdgeInfo> FlowGraph::edges() const {
    std::vector<EdgeInfo> out;
    for (std::size_t id = 0; id < arcs.size(); id += 2)
        out.push_back({arcs[id + 1].to, arcs[id].to, arcs[id].cap, arcs[id].flow});
    return out;
}

std::vector<bool> FlowGraph::minCutSourceSide() const {
    std::vector<bool> reach(adj.size(), false);
    if (source < 0) return reach;
    std::queue<int> q;
    q.push(source);
    reach[source] = true;
    while (!q.empty()) {
        const int u = q.front();
        q.pop();
        for (int id : adj[u]) {
            const Arc& a = arcs[id];
            if (!reach[a.to] && residualOf(a) > 0) {
                reach[a.to] = true;
                q.push(a.to);
            }
        }
    }
    return reach;
}

}  // namespace introalgo
