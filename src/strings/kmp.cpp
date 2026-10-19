#include "introalgo/strings/kmp.hpp"

#include <utility>

namespace introalgo {

KMP::KMP(std::string s1, std::string s2)
    : target(std::move(s1)),
      pattern(std::move(s2)),
      lps_(pattern.size(), 0),
      n(static_cast<int>(pattern.size())),
      m(static_cast<int>(target.size())) {
    getLPS();
}

void KMP::getLPS() {
    int len = 0;
    for (int i = 1; i < n; i++) {
        // Fall back through shorter borders until the next character extends one.
        while (len && pattern[i] != pattern[len]) len = lps_[len - 1];
        if (pattern[i] == pattern[len]) len++;
        lps_[i] = len;
    }
}

std::vector<int> KMP::calculateKMP() const {
    std::vector<int> ans;
    if (n == 0) return ans;
    int i = 0, j = 0;
    while (j < m) {
        if (target[j] == pattern[i]) {
            if (++i == n) {
                ans.push_back(j - n + 1);
                i = lps_[i - 1];  // keep going: the next match may overlap this one
            }
            j++;
        } else {
            i > 0 ? i = lps_[i - 1] : j++;
        }
    }
    return ans;
}

}  // namespace introalgo
