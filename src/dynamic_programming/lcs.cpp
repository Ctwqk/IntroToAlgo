#include "introalgo/dynamic_programming/lcs.hpp"

#include <utility>

namespace introalgo {

Lcs::Lcs(std::string s1, std::string s2)
    : str1(std::move(s1)),
      str2(std::move(s2)),
      size1(static_cast<int>(str1.size())),
      size2(static_cast<int>(str2.size())),
      tabulate(static_cast<std::size_t>(size2) + 1,
               std::vector<int>(static_cast<std::size_t>(size1) + 1, 0)) {}

std::string Lcs::getLCS() {
    for (int i = 1; i <= size2; i++) {
        for (int j = 1; j <= size1; j++) {
            if (str1[j - 1] == str2[i - 1])
                tabulate[i][j] = tabulate[i - 1][j - 1] + 1;
            else if (tabulate[i][j - 1] > tabulate[i - 1][j])
                tabulate[i][j] = tabulate[i][j - 1];
            else
                tabulate[i][j] = tabulate[i - 1][j];
        }
    }
    // Trace back from the bottom-right corner.  Row = position in str2,
    // column = position in str1.
    std::string ans;
    int index1 = size1, index2 = size2;
    while (index1 > 0 && index2 > 0) {
        if (str1[index1 - 1] == str2[index2 - 1]) {
            ans = str1[--index1] + ans;
            index2--;
        } else if (tabulate[index2 - 1][index1] >= tabulate[index2][index1 - 1]) {
            index2--;
        } else {
            index1--;
        }
    }
    return ans;
}

int Lcs::getLCSLength() const {
    // list[j] holds the previous row's value for column j while a row is being
    // computed; each cell is written one step late (list[j-1] = value of the
    // cell just finished) so that the diagonal neighbour is still available.
    std::vector<int> list(static_cast<std::size_t>(size1) + 1, 0);
    int tmp = 0, swa;
    for (int i = 1; i <= size2; i++) {
        tmp = 0;
        for (int j = 1; j <= size1; j++) {
            swa = tmp;
            if (str2[i - 1] == str1[j - 1])
                tmp = list[j - 1] + 1;
            else if (tmp < list[j])
                tmp = list[j];
            list[j - 1] = swa;
        }
        list[size1] = tmp;
    }
    return list[size1];
}

}  // namespace introalgo
