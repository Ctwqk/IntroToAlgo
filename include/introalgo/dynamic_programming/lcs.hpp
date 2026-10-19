#pragma once

#include <string>
#include <vector>

namespace introalgo {

/// Longest common subsequence of two strings.
///
/// Two flavours, as in the original program:
///  * getLCS()       fills the full (|str2|+1) x (|str1|+1) table and traces back
///                   one longest common subsequence.  O(nm) time, O(nm) space.
///  * getLCSLength() computes only the length with a single row of the table.
///                   O(nm) time, O(n) extra space. The full table is allocated only by
///                   getLCS() and retained after that call.
class Lcs {
public:
    Lcs(std::string s1, std::string s2);

    /// One longest common subsequence of the two strings.
    std::string getLCS();

    /// Length of the longest common subsequence (single-row dynamic program).
    int getLCSLength() const;

private:
    std::string str1, str2;
    int size1, size2;
    /// tabulate[i][j] = LCS length of str2[0, i) and str1[0, j).
    std::vector<std::vector<int>> tabulate;
};

}  // namespace introalgo
