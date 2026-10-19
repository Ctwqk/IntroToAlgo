#include "introalgo/dynamic_programming/lmis.hpp"

#include <algorithm>
#include <utility>

namespace introalgo {

Lmis::Lmis(std::vector<int> s) : sequence(std::move(s)) {}

int Lmis::findLMIS() {
    const std::size_t size = sequence.size();
    ans.assign(size, 0);
    prev.assign(size, -1);
    int best = 0;
    for (std::size_t i = 0; i < size; i++) {
        // The running maximum has to start from 0 for every i; the original kept
        // it across iterations and over-counted.
        int max = 0;
        for (std::size_t j = 0; j < i; j++) {
            if (sequence[j] < sequence[i] && ans[j] > max) {
                max = ans[j];
                prev[i] = static_cast<int>(j);
            }
        }
        ans[i] = max + 1;
        best = std::max(best, ans[i]);
    }
    return best;
}

std::vector<int> Lmis::subsequence() {
    const int best = findLMIS();
    std::vector<int> out;
    if (best == 0) return out;
    int end = static_cast<int>(std::max_element(ans.begin(), ans.end()) - ans.begin());
    for (int k = end; k != -1; k = prev[k]) out.push_back(sequence[k]);
    std::reverse(out.begin(), out.end());
    return out;
}

}  // namespace introalgo
