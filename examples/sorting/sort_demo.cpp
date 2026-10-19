// Original demo of sort.cpp (test_radix): radix sort of 15 random 5-digit records.  The random
// numbers now come from a seeded generator so that the output is reproducible.
#include <iostream>
#include <random>
#include <vector>

#include "introalgo/sorting/sort.hpp"

int main() {
    std::mt19937 rng(2025);
    std::vector<introalgo::DigitKey> records;
    for (int i = 0; i < 15; i++) {
        introalgo::DigitKey rec(5);
        for (int j = 0; j < 5; j++) rec.key[j] = static_cast<int>(rng() % 10);
        records.push_back(rec);
    }
    introalgo::Sort().radix_sort(records, 5);
    for (const auto& r : records) r.show(std::cout);

    introalgo::Sort s({3, 5, 1, 3, 5, 7, 8, 3, 2, 5, 10, 33, 44, 1323, 42, 99, 0, 1, 4, 29, 4, 5});
    s.counting_sort();
    std::cout << "counting sort: ";
    s.checkout(std::cout);
    std::cout << std::endl;
}
