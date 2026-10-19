#pragma once

#include <vector>

namespace introalgo {

/// Rod cutting (CLRS 15.1): given the price of a piece of every length
/// 1..n, find the cut that maximizes the total price of a rod of length n.
///
/// The original file only contained an empty `cutting` class with a
/// "just begin" comment; this fills it in with the bottom-up dynamic program.
/// O(n^2) time, O(n) space.
class RodCutting {
public:
    /// price[i] is the price of a piece of length i + 1.
    explicit RodCutting(std::vector<int> price);

    /// Best revenue for a rod of the given length (0 <= length <= price.size()).
    /// Both queries throw std::overflow_error if an optimal subproblem revenue exceeds int range.
    int bestRevenue(int length);

    /// Lengths of the pieces of one optimal cut of a rod of the given length.
    std::vector<int> cuts(int length);

private:
    std::vector<int> price;
    std::vector<int> revenue;   // revenue[l]: best revenue of a rod of length l
    std::vector<int> firstCut;  // firstCut[l]: length of the first piece in an optimal cut
    void fill(int length);
};

}  // namespace introalgo
