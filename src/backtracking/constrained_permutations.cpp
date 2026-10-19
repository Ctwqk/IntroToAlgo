#include "introalgo/backtracking/constrained_permutations.hpp"

#include <ostream>
#include <queue>
#include <stdexcept>

namespace introalgo {

ConstrainedPermutations::ConstrainedPermutations(int numbers, int increasing, int decreasing)
    : n(numbers), shd1(increasing), shd2(decreasing) {
    if (n < 0 || n > 12)
        throw std::invalid_argument("ConstrainedPermutations: n must be in [0, 12]");
}

void ConstrainedPermutations::show(std::ostream& os) const {  // output the answer of the question
    os << "question: " << n << " number " << shd1 << " increasing and " << shd2 << " decreasing;"
       << std::endl;
    os << "list of 100 possible answers:" << std::endl;
    for (std::size_t k = 0; k < 100 && k < ans.size(); k++) {
        for (int v : ans[k]) os << v << " ";
        os << std::endl;
    }
    os << "number of answers: " << ans.size() << std::endl;
}

void ConstrainedPermutations::find() {  // find possible lists
    ans.clear();
    if (n == 0) {
        ans.push_back({});
        return;
    }
    using State = std::pair<std::vector<int>, std::vector<std::pair<int, int>>>;
    std::queue<State> bfs;
    bfs.push(State());
    for (int layer = 0; layer < n; layer++) {
        const std::size_t size = bfs.size();
        for (std::size_t i = 0; i < size; i++) {
            State temp = bfs.front();
            bfs.pop();
            temp.first.push_back(0);
            temp.second.push_back(std::make_pair(1, 1));
            for (int j = 1; j <= n; j++) {
                State swap = temp;
                swap.first[layer] = j;
                if (judge(swap.first, layer, swap.second)) {
                    bfs.push(swap);
                    if (bfs.size() > kMaxStates)
                        throw std::length_error(
                            "ConstrainedPermutations: too many partial permutations");
                }
            }
        }
    }
    while (bfs.size()) {
        ans.push_back(bfs.front().first);
        bfs.pop();
    }
}

// Judges whether the current list is still valid.  id[k].first / id[k].second are the lengths of
// the longest increasing / decreasing subsequence that ends at position k; only id[idx] is computed
// here.
bool ConstrainedPermutations::judge(const std::vector<int>& stage, int idx,
                                    std::vector<std::pair<int, int>>& id) const {
    for (int i = 0; i < idx; i++) {
        if (stage[i] == stage[idx]) return false;
        if (stage[i] < stage[idx] && id[i].first >= id[idx].first) {
            id[idx].first = id[i].first + 1;
            if (id[idx].first > shd1) {
                return false;
            }
        }
        if (stage[i] > stage[idx] && id[i].second >= id[idx].second) {
            // (The original wrote id[idx].second + 1; for permutations that gives the same value,
            // because every element of a longest chain is itself an earlier predecessor.  This is
            // the textbook form of the recurrence.)
            id[idx].second = id[i].second + 1;
            if (id[idx].second > shd2) {
                return false;
            }
        }
    }
    // A single element already is an increasing and a decreasing subsequence of length 1, so
    // thresholds below 1 admit nothing (the original accepted position 0 unconditionally).
    return id[idx].first <= shd1 && id[idx].second <= shd2;
}

}  // namespace introalgo
