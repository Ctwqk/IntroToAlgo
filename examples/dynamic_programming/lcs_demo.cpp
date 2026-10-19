// Original demo of lcs.cpp: longest common subsequence of two bit strings.
#include <iostream>
#include <string>

#include "introalgo/dynamic_programming/lcs.hpp"

int main() {
    std::string str1 = "10010101";
    std::string str2 = "010110110";
    introalgo::Lcs l(str1, str2);
    std::cout << l.getLCSLength() << std::endl;
    std::cout << l.getLCS() << std::endl;
}
