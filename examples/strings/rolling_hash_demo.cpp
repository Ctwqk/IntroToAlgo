// Original demo of RollingHash.cpp: Rabin-Karp matching of a DNA pattern.
#include <iostream>
#include <string>

#include "introalgo/strings/rolling_hash.hpp"

int main() {
    std::string target =
        "TATTACTAGCTACTGTACTCGGCAGCCTATTCACTAGCTACTGTACTGGAGTACTATTCGGTGCGTATTCGACTAGCTACTGTACTGATA"
        "ACGTACGT";
    std::string pattern = "ACTAGCTACTGTACT";
    introalgo::DNAMatch dnam(pattern, target);
    for (int i : dnam.match()) std::cout << i << " ";
    std::cout << std::endl;
}
