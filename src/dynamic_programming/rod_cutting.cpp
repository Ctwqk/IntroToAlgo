#include "introalgo/dynamic_programming/rod_cutting.hpp"

#include <limits>
#include <stdexcept>
#include <utility>

namespace introalgo {

RodCutting::RodCutting(std::vector<int> p) : price(std::move(p)) {}

void RodCutting::fill(int length) {
    if (length < 0 || static_cast<std::size_t>(length) > price.size())
        throw std::out_of_range("RodCutting: no price for a piece that long");
    revenue.assign(static_cast<std::size_t>(length) + 1, 0);
    firstCut.assign(static_cast<std::size_t>(length) + 1, 0);
    for (int l = 1; l <= length; l++) {
        int best = std::numeric_limits<int>::min();
        for (int piece = 1; piece <= l; piece++) {
            const int candidate = price[piece - 1] + revenue[l - piece];
            if (candidate > best) {
                best = candidate;
                firstCut[l] = piece;
            }
        }
        revenue[l] = best;
    }
}

int RodCutting::bestRevenue(int length) {
    fill(length);
    return revenue[length];
}

std::vector<int> RodCutting::cuts(int length) {
    fill(length);
    std::vector<int> pieces;
    for (int l = length; l > 0; l -= firstCut[l]) pieces.push_back(firstCut[l]);
    return pieces;
}

}  // namespace introalgo
