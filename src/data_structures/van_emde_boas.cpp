#include "introalgo/data_structures/van_emde_boas.hpp"

#include <stdexcept>
#include <utility>

namespace introalgo {

VanEmdeBoas::VanEmdeBoas(int universe) {
    if (universe < 0 || universe > (1 << 30))
        throw std::invalid_argument("VanEmdeBoas: universe must be in [0, 2^30]");
    int bits = 1;
    while ((1LL << bits) < universe) bits++;
    u = 1 << bits;
    lowBits = bits / 2;
    if (u > 2) {
        const int clusters = 1 << (bits - lowBits);
        summary.reset(new VanEmdeBoas(clusters));
        cluster.resize(static_cast<std::size_t>(clusters));
    }
}

VanEmdeBoas& VanEmdeBoas::clusterAt(int h) {
    auto& slot = cluster[static_cast<std::size_t>(h)];
    if (!slot) slot.reset(new VanEmdeBoas(1 << lowBits));
    return *slot;
}

bool VanEmdeBoas::member(int x) const {
    if (x < 0 || x >= u) return false;
    if (min == x || max == x) return true;
    if (u == 2) return false;
    const auto& c = cluster[static_cast<std::size_t>(high(x))];
    return c && c->member(low(x));
}

int VanEmdeBoas::successor(int x) const {
    if (x < 0) return min;
    if (x >= u - 1) return -1;
    if (u == 2) return (x == 0 && max == 1) ? 1 : -1;
    if (min != -1 && x < min) return min;
    const auto& c = cluster[static_cast<std::size_t>(high(x))];
    const int maxLow = c ? c->maximum() : -1;
    if (maxLow != -1 && low(x) < maxLow) return idx(high(x), c->successor(low(x)));
    const int succCluster = summary->successor(high(x));
    if (succCluster == -1) return -1;
    return idx(succCluster, cluster[static_cast<std::size_t>(succCluster)]->minimum());
}

int VanEmdeBoas::predecessor(int x) const {
    if (x >= u) return max;
    if (x <= 0) return -1;
    if (u == 2) return (x == 1 && min == 0) ? 0 : -1;
    if (max != -1 && x > max) return max;
    const auto& c = cluster[static_cast<std::size_t>(high(x))];
    const int minLow = c ? c->minimum() : -1;
    if (minLow != -1 && low(x) > minLow) return idx(high(x), c->predecessor(low(x)));
    const int predCluster = summary->predecessor(high(x));
    if (predCluster == -1) return (min != -1 && x > min) ? min : -1;  // the minimum is stored apart
    return idx(predCluster, cluster[static_cast<std::size_t>(predCluster)]->maximum());
}

bool VanEmdeBoas::emptyInsert(int x) {
    max = x;
    min = x;
    return true;
}

bool VanEmdeBoas::insert(int x) {
    if (x < 0 || x >= u || member(x)) return false;
    return insertImpl(x);
}

bool VanEmdeBoas::insertImpl(int x) {
    if (min == -1) return emptyInsert(x);
    if (x < min) std::swap(x, min);  // the old minimum moves down into the clusters
    if (u > 2) {
        const auto& c = cluster[static_cast<std::size_t>(high(x))];
        if (!c || c->empty()) {
            summary->insertImpl(high(x));
            clusterAt(high(x)).emptyInsert(low(x));
        } else {
            c->insertImpl(low(x));
        }
    }
    if (x > max) max = x;
    return true;
}

bool VanEmdeBoas::del(int x) {
    if (!member(x)) return false;
    return delImpl(x);
}

bool VanEmdeBoas::delImpl(int x) {
    if (min == max) {  // x is the only element
        min = max = -1;
        return true;
    }
    if (u == 2) {
        min = (x == 0) ? 1 : 0;
        max = min;
        return true;
    }
    if (x == min) {
        // Pull the smallest element of the clusters up to become the new minimum, then delete
        // that element from its cluster.
        const int firstCluster = summary->minimum();
        x = idx(firstCluster, cluster[static_cast<std::size_t>(firstCluster)]->minimum());
        min = x;
    }
    auto& c = cluster[static_cast<std::size_t>(high(x))];
    c->delImpl(low(x));
    if (c->empty()) {
        c.reset();  // free the empty cluster
        summary->delImpl(high(x));
        if (x == max) {
            const int summaryMax = summary->maximum();
            if (summaryMax == -1)
                max = min;
            else
                max = idx(summaryMax, cluster[static_cast<std::size_t>(summaryMax)]->maximum());
        }
    } else if (x == max) {
        max = idx(high(x), c->maximum());
    }
    return true;
}

}  // namespace introalgo
