// Original demo of LongSinglePath.cpp: shortest bitonic tour through 7 points.
#include <iostream>
#include <vector>

#include "introalgo/dynamic_programming/bitonic_tour.hpp"

int main() {
    std::vector<introalgo::Point> cops{{0, 6}, {1, 0}, {2, 3}, {5, 4}, {6, 1}, {7, 5}, {8, 2}};
    introalgo::BitonicTour t(cops);
    t.solve();
    std::cout << "shortest bitonic tour length: " << t.length() << std::endl;
    std::cout << "tour:";
    for (int idx : t.tour()) std::cout << " (" << cops[idx].x << "," << cops[idx].y << ")";
    std::cout << " (back to start)" << std::endl;
}
