#include <gtest/gtest.h>

#include <algorithm>
#include <limits>
#include <numeric>
#include <sstream>
#include <vector>

#include "introalgo/dynamic_programming/obst.hpp"
#include "test_utils.hpp"

using introalgo::Obst;

namespace {

// One concrete binary search tree on a range of keys, as (index, depth) pairs relative to its root.
// enumerate() produces every such tree (Catalan(n) of them).
struct SubTree {
    std::vector<std::pair<int, int>> keys;     // (key index, depth)
    std::vector<std::pair<int, int>> dummies;  // (dummy index, depth)
};

std::vector<SubTree> enumerate(int i, int j) {
    if (i > j) return {SubTree{{}, {{i - 1, 0}}}};  // a lone dummy key d_{i-1}
    std::vector<SubTree> out;
    for (int r = i; r <= j; ++r) {
        for (const auto& left : enumerate(i, r - 1)) {
            for (const auto& right : enumerate(r + 1, j)) {
                SubTree t;
                t.keys.push_back({r, 0});
                for (auto k : left.keys) t.keys.push_back({k.first, k.second + 1});
                for (auto k : right.keys) t.keys.push_back({k.first, k.second + 1});
                for (auto d : left.dummies) t.dummies.push_back({d.first, d.second + 1});
                for (auto d : right.dummies) t.dummies.push_back({d.first, d.second + 1});
                out.push_back(std::move(t));
            }
        }
    }
    return out;
}

// Expected search cost of a concrete tree: sum p_i (depth_i + 1) + sum q_i (depth(d_i) + 1)
// where the root has depth 0 (CLRS definition).
double costOf(const SubTree& t, const std::vector<double>& p, const std::vector<double>& q) {
    double c = 0;
    for (auto k : t.keys) c += p[k.first - 1] * (k.second + 1);
    for (auto d : t.dummies) c += q[d.first] * (d.second + 1);
    return c;
}

double bruteForceCost(const std::vector<double>& p, const std::vector<double>& q) {
    double best = std::numeric_limits<double>::infinity();
    for (const auto& t : enumerate(1, static_cast<int>(p.size())))
        best = std::min(best, costOf(t, p, q));
    return best;
}

std::vector<double> randomDistribution(testutil::Rng& rng, std::size_t count) {
    std::vector<double> v(count);
    for (auto& x : v) x = testutil::randReal(rng, 0.01, 1.0);
    return v;
}

}  // namespace

TEST(Obst, ClrsExample) {
    // CLRS figure 15.9: expected search cost 2.75, root k2.
    Obst o({0.15, 0.10, 0.05, 0.10, 0.20}, {0.05, 0.10, 0.05, 0.05, 0.05, 0.10});
    o.find_obst2();
    EXPECT_NEAR(o.expectedCost(), 2.75, 1e-9);
    EXPECT_EQ(o.rootOf(1, 5), 2);
}

TEST(Obst, OriginalDemoOutput) {
    Obst o({0.04, 0.06, 0.08, 0.02, 0.1, 0.12, 0.14},
           {0.06, 0.06, 0.06, 0.06, 0.05, 0.05, 0.05, 0.05});
    o.find_obst2();
    EXPECT_NEAR(o.expectedCost(), 3.12, 1e-9);
    EXPECT_EQ(o.rootOf(1, 7), 5);
    std::ostringstream out;
    o.show_obst(out);
    EXPECT_NE(out.str().find("root is k5, the OBST expectation is 3.12"), std::string::npos);
    EXPECT_NE(out.str().find("the left child of k5 is k2"), std::string::npos);
    EXPECT_NE(out.str().find("the right child of k5 is k7"), std::string::npos);
}

TEST(Obst, SingleKeyAndNoKeys) {
    Obst one({0.5}, {0.2, 0.3});
    one.find_obst2();
    EXPECT_NEAR(one.expectedCost(), 0.5 * 1 + 0.2 * 2 + 0.3 * 2, 1e-12);
    EXPECT_EQ(one.rootOf(1, 1), 1);

    Obst none({}, {1.0});
    none.find_obst2();
    EXPECT_NEAR(none.expectedCost(), 1.0, 1e-12);
    std::ostringstream out;
    none.show_obst(out);
    EXPECT_FALSE(out.str().empty());
}

TEST(Obst, RejectsInconsistentInput) {
    EXPECT_THROW(Obst({0.5, 0.5}, {0.5, 0.5}), std::invalid_argument);
    EXPECT_THROW(Obst({}, {}), std::invalid_argument);
}

TEST(Obst, RandomizedMatchesEnumerationOfAllTrees) {
    testutil::Rng rng(7);
    for (int iter = 0; iter < 150; ++iter) {
        const std::size_t n = std::size_t(testutil::randInt(rng, 1, 7));
        auto p = randomDistribution(rng, n);
        auto q = randomDistribution(rng, n + 1);
        Obst o(p, q);
        o.find_obst2();
        EXPECT_NEAR(o.expectedCost(), bruteForceCost(p, q), 1e-9);

        // The reconstructed tree must itself be a valid BST whose cost equals the table value.
        const auto kd = o.keyDepths();
        const auto dd = o.dummyDepths();
        double cost = 0;
        for (std::size_t i = 0; i < n; ++i) cost += p[i] * (kd[i] + 1);
        for (std::size_t i = 0; i <= n; ++i) cost += q[i] * (dd[i] + 1);
        EXPECT_NEAR(cost, o.expectedCost(), 1e-9);
        // Exactly one root, and the dummy keys are all leaves one level below some key.
        EXPECT_EQ(std::count(kd.begin(), kd.end(), 0), 1);
        EXPECT_GE(*std::min_element(dd.begin(), dd.end()), 1);
    }
}
