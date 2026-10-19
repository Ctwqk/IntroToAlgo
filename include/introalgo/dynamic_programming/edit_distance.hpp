#pragma once

#include <string>

namespace introalgo {

/// Levenshtein edit distance: the minimum number of single-character
/// insertions, deletions and substitutions that turn `s1` into `s2`.
/// O(nm) time and space.
class ED {
public:
    ED(std::string s1, std::string s2);

    int calculate() const;

private:
    std::string x;
    std::string y;
};

}  // namespace introalgo
