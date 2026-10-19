// Original demo of KMP.cpp: find a DNA pattern in a DNA text.
#include <iostream>
#include <string>
#include <vector>

#include "introalgo/strings/kmp.hpp"

int main() {
    std::string s1 =
        "TATTACTAGCTACTGTACTCGGCAGCCTATTCACTAGCTACTGTACTGGAGTACTATTCGGTGCGTATTCGACTAGCTACTGTACTGATA"
        "ACGTACGT";
    std::string s2 = "ACTAGCTACTGTACT";
    introalgo::KMP k(s1, s2);
    std::vector<int> ans = k.calculateKMP();
    for (std::size_t i = 0; i < ans.size(); i++) std::cout << ans[i] << " ";
    std::cout << std::endl;
}
