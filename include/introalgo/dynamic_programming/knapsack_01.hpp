#pragma once

#include <utility>
#include <vector>

namespace introalgo {

/// 0/1 knapsack: every item can be taken at most once.
/// Items are (value, weight) pairs.  O(n * capacity) time and space.
class Knapsack {
public:
    Knapsack(std::vector<std::pair<int, int>> value_weight, int capacity);

    /// Maximum total value that fits into the capacity.
    int solve();

    /// Indices of one optimal set of items (valid after solve()).
    std::vector<int> chosenItems() const;

private:
    int size;
    int capacity;  // weight capacity
    std::vector<std::pair<int, int>> value_weight;
    std::vector<std::vector<int>>
        dptab;  // dptab[i][j]: best value using the first i items, capacity j
};

}  // namespace introalgo
