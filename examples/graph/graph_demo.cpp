// Original demos of graph.cpp (test_ufset and test_graph): the union-find ranks of 20 singletons,
// and the 11-vertex undirected example with its all-pairs distance matrix.
#include <iostream>
#include <vector>

#include "introalgo/graph/graph.hpp"

using introalgo::Graph;

int main() {
    introalgo::UFSet uf(20);
    uf.show(std::cout);
    std::cout << std::endl;

    std::vector<std::vector<int>> arr = {
        {0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0}, {1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0},
        {1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0}, {0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0},
        {1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0}, {0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
        {0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1}, {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0}};
    Graph g(arr);
    g.showMat(std::cout);
    std::cout << "components: " << g.componentCount()
              << ", directed: " << (g.isDirected() ? "yes" : "no")
              << ", has cycle: " << (g.isCycle() ? "yes" : "no") << std::endl;
    std::cout << "BFS from 0:";
    for (int v : g.BFS(0)) std::cout << " " << v;
    std::cout << std::endl;
    std::cout << "----------------------------after------------------------" << std::endl;
    const auto d = g.multmat();
    for (const auto& row : d) {
        for (long long x : row) {
            if (x == Graph::kInfinity)
                std::cout << " . ";
            else
                std::cout << " " << x << " ";
        }
        std::cout << std::endl;
    }
}
