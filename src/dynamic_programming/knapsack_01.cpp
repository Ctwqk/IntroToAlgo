#include "introalgo/dynamic_programming/knapsack_01.hpp"

#include <algorithm>
#include <limits>
#include <stdexcept>
#include <utility>

namespace introalgo {

Knapsack::Knapsack(std::vector<std::pair<int, int>> v_w, int cap)
    : size(static_cast<int>(v_w.size())),
      capacity(std::max(cap, 0)),
      value_weight(std::move(v_w)),
      dptab(static_cast<std::size_t>(size) + 1,
            std::vector<int>(static_cast<std::size_t>(capacity) + 1, 0)) {
    for (const auto& item : value_weight)
        if (item.second < 0) throw std::invalid_argument("Knapsack: negative weight");
}

int Knapsack::solve() {
    for (int i = 1; i <= size; i++) {
        const int value = value_weight[i - 1].first;
        const int weight = value_weight[i - 1].second;
        for (int j = 0; j <= capacity; j++) {
            // An item that does not fit leaves the best value of the previous row
            // (the original left a 0 here and patched the answer afterwards).
            dptab[i][j] = dptab[i - 1][j];
            if (weight <= j) {
                const long long candidate =
                    static_cast<long long>(dptab[i - 1][j - weight]) + value;
                if (candidate > std::numeric_limits<int>::max())
                    throw std::overflow_error("Knapsack: value exceeds int range");
                dptab[i][j] = std::max(dptab[i][j], static_cast<int>(candidate));
            }
        }
    }
    return dptab[size][capacity];
}

std::vector<int> Knapsack::chosenItems() const {
    std::vector<int> items;
    int j = capacity;
    for (int i = size; i >= 1; i--) {
        if (dptab[i][j] != dptab[i - 1][j]) {  // item i-1 was needed
            items.push_back(i - 1);
            j -= value_weight[i - 1].second;
        }
    }
    std::reverse(items.begin(), items.end());
    return items;
}

}  // namespace introalgo
