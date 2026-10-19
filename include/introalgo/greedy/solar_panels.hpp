#pragma once

#include <vector>

namespace introalgo {

/// Greedy matching of solar-panel packages to houses (the original `maximum_num_houses_solar`
/// from sy.cpp).
///
/// House i needs at least `houseMin[i]` panels.  Package j has `solarPanel[j]` panels and can be
/// installed on at most `packageTime[j]` houses (one house gets one package).  What is the largest
/// number of houses that can be served?
///
/// Greedy rule: serve the houses in increasing order of demand and give each the smallest package
/// that is still big enough and has installations left; stop at the first house nobody can serve
/// (every later house needs even more).  This is optimal -- the sets of packages that can serve
/// the houses are nested, and using the smallest suitable package leaves the large ones for the
/// demanding houses.  O((n + m) log(n + m) + m n).
class MaximumNumHousesSolar {
public:
    /// `solarPanel` and `packageTime` have one entry per package (std::invalid_argument otherwise).
    MaximumNumHousesSolar(std::vector<int> solarPanel, std::vector<int> packageTime,
                          std::vector<int> houseMin);

    /// The largest number of houses that can be served (called mac_num_house in the original).
    int maxNumHouses() const;

private:
    std::vector<int> numOfSolarPanels;
    std::vector<int> timeOfPackageCanBeUsed;
    std::vector<int> minimumHouseNeeded;
};

}  // namespace introalgo
