#include "introalgo/dynamic_programming/obst.hpp"

#include <ostream>
#include <queue>
#include <stdexcept>
#include <utility>

namespace introalgo {

Obst::Obst(const std::vector<double>& a, const std::vector<double>& b)
    : size(static_cast<int>(b.size())) {
    if (b.empty() || a.size() + 1 != b.size())
        throw std::invalid_argument("Obst: need n key probabilities and n + 1 dummy probabilities");
    p.assign(static_cast<std::size_t>(size), 0.0);
    q.assign(static_cast<std::size_t>(size), 0.0);
    for (int i = 1; i < size; i++) {
        p[i] = a[i - 1];
        q[i] = b[i];
    }
    q[0] = b[0];
    root.assign(static_cast<std::size_t>(size),
                std::vector<int>(static_cast<std::size_t>(size), 0));
    e.assign(static_cast<std::size_t>(size) + 1,
             std::vector<double>(static_cast<std::size_t>(size) + 1, 0.0));
    w = e;
    for (int i = 1; i < size + 1; i++) {
        e[i][i - 1] = q[i - 1];
        w[i][i - 1] = q[i - 1];
    }
}

void Obst::find_obst2() {
    for (int k = 0; k < size; k++) {
        for (int i = 1; i < size - k; i++) {
            int j = i + k;
            w[i][j] = p[j] + q[j] + w[i][j - 1];
            e[i][j] = e[i][i - 1] + e[i + 1][j] + w[i][j];
            root[i - 1][j - 1] = i;
            for (int l = i + 1; l <= j; l++) {
                if (e[i][l - 1] + e[l + 1][j] + w[i][j] < e[i][j]) {
                    e[i][j] = e[i][l - 1] + e[l + 1][j] + w[i][j];
                    root[i - 1][j - 1] = l;
                }
            }
        }
    }
}

double Obst::expectedCost() const {
    return e[1][size - 1];
}

int Obst::rootOf(int i, int j) const {
    if (i < 1 || j < i || j > size - 1) throw std::out_of_range("Obst::rootOf");
    return root[i - 1][j - 1];
}

void Obst::collectDepths(int i, int j, int depth, std::vector<int>& keys,
                         std::vector<int>& dummies) const {
    if (i > j) {  // empty range of keys: the dummy key d_{i-1} hangs here
        dummies[i - 1] = depth;
        return;
    }
    const int r = root[i - 1][j - 1];
    keys[r - 1] = depth;
    collectDepths(i, r - 1, depth + 1, keys, dummies);
    collectDepths(r + 1, j, depth + 1, keys, dummies);
}

std::vector<int> Obst::keyDepths() const {
    std::vector<int> keys(static_cast<std::size_t>(size - 1), 0),
        dummies(static_cast<std::size_t>(size), 0);
    collectDepths(1, size - 1, 0, keys, dummies);
    return keys;
}

std::vector<int> Obst::dummyDepths() const {
    std::vector<int> keys(static_cast<std::size_t>(size - 1), 0),
        dummies(static_cast<std::size_t>(size), 0);
    collectDepths(1, size - 1, 0, keys, dummies);
    return dummies;
}

void Obst::show_obst(std::ostream& os) const {
    if (size == 1) {
        os << "the OBST has no keys, the expectation is " << e[1][0] << std::endl;
        return;
    }
    std::queue<std::pair<int, int>> bfs;
    bfs.push(std::make_pair(0, size - 2));
    std::pair<int, int> tmp;
    int ro;
    os << "root is k" << root[0][size - 2] << ", the OBST expectation is " << e[1][size - 1]
       << std::endl;
    while (bfs.size()) {
        tmp = bfs.front();
        bfs.pop();
        ro = root[tmp.first][tmp.second] - 1;
        if (tmp.first == tmp.second) {
            os << "the left child of k" << tmp.first + 1 << " is d" << tmp.second << std::endl;
            os << "the right child of k" << tmp.first + 1 << " is d" << tmp.second + 1 << std::endl;
        } else if (ro == tmp.first) {
            os << "the left child of k" << ro + 1 << " is d" << ro << std::endl;
            os << "the right child of k" << ro + 1 << " is k" << root[ro + 1][tmp.second]
               << std::endl;
            bfs.push(std::make_pair(ro + 1, tmp.second));
        } else if (ro == tmp.second) {
            os << "the right child of k" << ro + 1 << " is d" << ro + 1 << std::endl;
            os << "the left child of k" << ro + 1 << " is k" << root[tmp.first][ro - 1]
               << std::endl;
            bfs.push(std::make_pair(tmp.first, ro - 1));
        } else {
            bfs.push(std::make_pair(tmp.first, ro - 1));
            bfs.push(std::make_pair(ro + 1, tmp.second));
            os << "the left child of k" << ro + 1 << " is k" << root[tmp.first][ro - 1]
               << std::endl;
            os << "the right child of k" << ro + 1 << " is k" << root[ro + 1][tmp.second]
               << std::endl;
        }
    }
}

}  // namespace introalgo
