#include "introalgo/greedy/solar_panels.hpp"

#include <algorithm>
#include <functional>
#include <queue>
#include <stdexcept>
#include <utility>

namespace introalgo {

MaximumNumHousesSolar::MaximumNumHousesSolar(std::vector<int> solarPanel,
                                             std::vector<int> packageTime,
                                             std::vector<int> houseMin)
    : numOfSolarPanels(std::move(solarPanel)),
      timeOfPackageCanBeUsed(std::move(packageTime)),
      minimumHouseNeeded(std::move(houseMin)) {
    if (numOfSolarPanels.size() != timeOfPackageCanBeUsed.size())
        throw std::invalid_argument(
            "MaximumNumHousesSolar: one usage count per package is required");
}

int MaximumNumHousesSolar::maxNumHouses() const {
    std::priority_queue<int, std::vector<int>, std::greater<int>> houses;
    for (int need : minimumHouseNeeded) houses.push(need);
    std::vector<std::pair<int, int>> packages;
    for (std::size_t i = 0; i < numOfSolarPanels.size(); i++)
        packages.push_back(std::make_pair(numOfSolarPanels[i], timeOfPackageCanBeUsed[i]));
    std::sort(packages.begin(), packages.end());
    const std::size_t n = packages.size();
    int ans = 0;
    while (houses.size()) {
        int tmp = houses.top();
        houses.pop();
        std::size_t i = 0;
        for (; i < n; i++) {
            if (tmp <= packages[i].first && packages[i].second > 0) {
                packages[i].second--;
                break;
            }
        }
        if (i < n)
            ans++;
        else
            break;
    }
    return ans;
}

}  // namespace introalgo
