// The original flow_graph.cpp (unfinished, did not compile) had no main(); this runs the CLRS
// figure 26.1 network, with vertices labelled by name.
#include <iostream>
#include <string>

#include "introalgo/graph/flow_graph.hpp"

int main() {
    introalgo::LabeledFlowGraph<std::string> g;
    g.addEdge("s", "v1", 16);
    g.addEdge("s", "v2", 13);
    g.addEdge("v1", "v2", 10);
    g.addEdge("v2", "v1", 4);
    g.addEdge("v1", "v3", 12);
    g.addEdge("v3", "v2", 9);
    g.addEdge("v2", "v4", 14);
    g.addEdge("v4", "v3", 7);
    g.addEdge("v3", "t", 20);
    g.addEdge("v4", "t", 4);
    std::cout << "max flow s -> t: " << g.maxFlow("s", "t") << std::endl;
    for (const auto& e : g.underlying().edges())
        if (e.flow > 0)
            std::cout << "  " << g.label(e.from) << " -> " << g.label(e.to) << ": " << e.flow << "/"
                      << e.capacity << std::endl;
}
