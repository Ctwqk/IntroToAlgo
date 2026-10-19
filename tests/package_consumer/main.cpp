// Smoke test for the installed package: calls into two different modules of the library.
#include <iostream>

#include <introalgo/introalgo.hpp>

int main() {
    introalgo::Lcs lcs("ABCBDAB", "BDCABA");
    introalgo::FlowGraph flow(2);
    flow.addEdge(0, 1, 7);
    const bool ok = lcs.getLCSLength() == 4 && flow.maxFlow(0, 1) == 7;
    std::cout << "introalgo package works: " << (ok ? "yes" : "NO") << std::endl;
    return ok ? 0 : 1;
}
