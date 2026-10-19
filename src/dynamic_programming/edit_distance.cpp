#include "introalgo/dynamic_programming/edit_distance.hpp"

#include <algorithm>
#include <utility>
#include <vector>

namespace introalgo {

ED::ED(std::string s1, std::string s2) : x(std::move(s1)), y(std::move(s2)) {}

int ED::calculate() const {
    const std::size_t n = x.size(), m = y.size();
    // dp[i][j] = edit distance between x[0, i) and y[0, j).
    std::vector<std::vector<int>> dp(n + 1, std::vector<int>(m + 1, 0));
    for (std::size_t i = 0; i <= n; i++) dp[i][0] = static_cast<int>(i);
    for (std::size_t j = 0; j <= m; j++) dp[0][j] = static_cast<int>(j);
    for (std::size_t i = 1; i <= n; i++) {
        for (std::size_t j = 1; j <= m; j++) {
            const int substitution = dp[i - 1][j - 1] + (x[i - 1] == y[j - 1] ? 0 : 1);
            dp[i][j] = std::min(substitution, std::min(dp[i - 1][j], dp[i][j - 1]) + 1);
        }
    }
    return dp[n][m];
}

}  // namespace introalgo
