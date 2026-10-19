// Original demo of knapsack_0_1.cpp: the classic (value, weight) instance with capacity 50.
#include <iostream>
#include <utility>
#include <vector>

#include "introalgo/dynamic_programming/knapsack_01.hpp"

int main() {
    std::vector<std::pair<int, int>> items{{100, 20}, {120, 30}, {60, 10}};
    introalgo::Knapsack k(items, 50);
    std::cout << "best value: " << k.solve() << std::endl;
    std::cout << "items taken (0-based):";
    for (int i : k.chosenItems()) std::cout << " " << i;
    std::cout << std::endl;
}
