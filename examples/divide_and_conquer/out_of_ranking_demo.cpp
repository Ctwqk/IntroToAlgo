// Original demo of DandC/OutOfRanking.cpp: how different are two rankings?
#include <iostream>
#include <vector>

#include "introalgo/divide_and_conquer/out_of_ranking.hpp"

int main() {
    std::vector<int> RankA{2, 4, 5, 1, 3, 6, 7};
    introalgo::OutOfRanking o;
    std::cout << "similarity is: " << o.user_similarity(RankA) << std::endl;
}
