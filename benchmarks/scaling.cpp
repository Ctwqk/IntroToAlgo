// Empirical complexity check: times each algorithm on doubling input sizes and fits the exponent
// k of  time ~ n^k  by least squares on the log-log data.  A quadratic algorithm should show
// k close to 2, a linear one close to 1, and an n log n algorithm a little above 1.
//
//   ./scaling            full run (about a minute in a Release build)
//   ./scaling --quick    tiny sizes, only checks that everything runs (used by ctest)
//   ./scaling --csv      machine-readable output
//
// This is a measurement tool, not a test: timings depend on the machine, so nothing here fails
// on a slow or noisy run.
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <functional>
#include <memory>
#include <numeric>
#include <random>
#include <string>
#include <vector>

#include "introalgo/data_structures/bst.hpp"
#include "introalgo/data_structures/max_heap.hpp"
#include "introalgo/data_structures/rb_tree.hpp"
#include "introalgo/data_structures/van_emde_boas.hpp"
#include "introalgo/divide_and_conquer/out_of_ranking.hpp"
#include "introalgo/dynamic_programming/edit_distance.hpp"
#include "introalgo/dynamic_programming/lcs.hpp"
#include "introalgo/graph/flow_graph.hpp"
#include "introalgo/graph/graph.hpp"
#include "introalgo/linear_programming/simplex.hpp"
#include "introalgo/matrix/lup.hpp"
#include "introalgo/sorting/sort.hpp"
#include "introalgo/strings/kmp.hpp"
#include "introalgo/strings/rolling_hash.hpp"

namespace {

using Clock = std::chrono::steady_clock;
volatile long long sink = 0;  // keeps the optimiser from deleting the work

struct Bench {
    std::string name;
    std::string expected;           // the textbook complexity, for the table
    double expectedExponent;        // what the fit should be close to (NaN: no sharp prediction)
    std::vector<int> sizes;
    std::vector<int> quickSizes;
    // Prepares the input for size n (not timed) and returns the function to time.
    std::function<std::function<void()>(int)> setup;
};

std::mt19937_64 rng(12345);

std::string randomString(int n, const std::string& alphabet) {
    std::string s(static_cast<std::size_t>(n), ' ');
    for (auto& c : s) c = alphabet[rng() % alphabet.size()];
    return s;
}

std::vector<int> randomInts(int n, int range = 1 << 30) {
    std::vector<int> v(static_cast<std::size_t>(n));
    for (auto& x : v) x = static_cast<int>(rng() % static_cast<unsigned>(range));
    return v;
}

// Doubles the size `steps` times.
std::vector<int> doubling(int start, int steps) {
    std::vector<int> v;
    for (int i = 0, n = start; i < steps; ++i, n *= 2) v.push_back(n);
    return v;
}

double seconds(const std::function<void()>& work) {
    double best = 1e300;
    for (int rep = 0; rep < 3; ++rep) {  // best of three: the least disturbed run
        const auto t0 = Clock::now();
        work();
        const auto t1 = Clock::now();
        best = std::min(best, std::chrono::duration<double>(t1 - t0).count());
    }
    return best;
}

// Least-squares slope of log(time) against log(n).
double fitExponent(const std::vector<int>& n, const std::vector<double>& t) {
    const std::size_t k = n.size();
    double sx = 0, sy = 0, sxx = 0, sxy = 0;
    for (std::size_t i = 0; i < k; ++i) {
        const double x = std::log(static_cast<double>(n[i])), y = std::log(std::max(t[i], 1e-9));
        sx += x;
        sy += y;
        sxx += x * x;
        sxy += x * y;
    }
    return (static_cast<double>(k) * sxy - sx * sy) / (static_cast<double>(k) * sxx - sx * sx);
}

std::vector<Bench> makeBenches() {
    using introalgo::DigitKey;
    using Work = std::function<void()>;
    std::vector<Bench> b;

    b.push_back({"LCS length (one-row DP)", "O(n^2)", 2.0, doubling(500, 5), doubling(50, 3), [](int n) -> Work {
                     auto a = std::make_shared<std::string>(randomString(n, "ACGT"));
                     auto c = std::make_shared<std::string>(randomString(n, "ACGT"));
                     return [a, c] { sink = sink + introalgo::Lcs(*a, *c).getLCSLength(); };
                 }});
    b.push_back({"Edit distance", "O(n^2)", 2.0, doubling(500, 5), doubling(50, 3), [](int n) -> Work {
                     auto a = std::make_shared<std::string>(randomString(n, "ACGT"));
                     auto c = std::make_shared<std::string>(randomString(n, "ACGT"));
                     return [a, c] { sink = sink + introalgo::ED(*a, *c).calculate(); };
                 }});
    b.push_back({"KMP search (worst-case pattern)", "O(n)", 1.0, doubling(1 << 17, 5), doubling(1 << 10, 3), [](int n) -> Work {
                     auto text = std::make_shared<std::string>(static_cast<std::size_t>(n), 'a');
                     auto pat = std::make_shared<std::string>(std::string(1000, 'a') + "b");
                     return [text, pat] { sink = sink + static_cast<long long>(introalgo::KMP(*text, *pat).calculateKMP().size()); };
                 }});
    b.push_back({"Rabin-Karp DNA match", "O(n) expected", 1.0, doubling(1 << 17, 5), doubling(1 << 10, 3), [](int n) -> Work {
                     auto text = std::make_shared<std::string>(randomString(n, "ACGT"));
                     auto pat = std::make_shared<std::string>(randomString(200, "ACGT"));
                     return [text, pat] { sink = sink + static_cast<long long>(introalgo::DNAMatch(*pat, *text).match().size()); };
                 }});
    b.push_back({"Inversion count (merge sort)", "O(n log n)", 1.1, doubling(1 << 16, 5), doubling(1 << 8, 3), [](int n) -> Work {
                     auto v = std::make_shared<std::vector<int>>(randomInts(n));
                     return [v] {
                         auto copy = *v;
                         sink = sink + introalgo::OutOfRanking().user_similarity(copy);
                     };
                 }});
    b.push_back({"Quick sort (random input)", "O(n log n)", 1.1, doubling(1 << 16, 5), doubling(1 << 8, 3), [](int n) -> Work {
                     auto v = std::make_shared<std::vector<int>>(randomInts(n));
                     return [v] {
                         introalgo::Sort s(*v);
                         s.quick_sort();
                         sink = sink + s.data().front();
                     };
                 }});
    b.push_back({"Insertion sort (random input)", "O(n^2)", 2.0, doubling(1000, 5), doubling(100, 3), [](int n) -> Work {
                     auto v = std::make_shared<std::vector<int>>(randomInts(n));
                     return [v] {
                         introalgo::Sort s(*v);
                         s.insert_sort();
                         sink = sink + s.data().front();
                     };
                 }});
    b.push_back({"Counting sort (values < 1000)", "O(n + k)", 1.0, doubling(1 << 17, 5), doubling(1 << 10, 3), [](int n) -> Work {
                     auto v = std::make_shared<std::vector<int>>(randomInts(n, 1000));
                     return [v] {
                         introalgo::Sort s(*v);
                         s.counting_sort();
                         sink = sink + s.data().front();
                     };
                 }});
    b.push_back({"Heap: n inserts + n pops", "O(n log n)", 1.1, doubling(1 << 15, 5), doubling(1 << 8, 3), [](int n) -> Work {
                     auto v = std::make_shared<std::vector<int>>(randomInts(n));
                     return [v] {
                         introalgo::Heap h(8);
                         for (int x : *v) h.insert(x);
                         while (!h.empty()) {
                             sink = sink + h.peek().getKey();
                             h.pop();
                         }
                     };
                 }});
    b.push_back({"BST: n sorted inserts (degenerate)", "O(n^2)", 2.0, doubling(2000, 5), doubling(100, 3), [](int n) -> Work {
                     return [n] {
                         introalgo::Bst t;
                         for (int i = 0; i < n; ++i) t.insert(i);
                         sink = sink + t.height();
                     };
                 }});
    b.push_back({"Red-black tree: n sorted inserts", "O(n log n)", 1.1, doubling(1 << 15, 5), doubling(1 << 8, 3), [](int n) -> Work {
                     return [n] {
                         introalgo::RbTree t;
                         for (int i = 0; i < n; ++i) t.insert(i);
                         sink = sink + t.height();
                     };
                 }});
    b.push_back({"van Emde Boas: n inserts + successor", "O(n log log u)", 1.0, doubling(1 << 15, 5), doubling(1 << 8, 3), [](int n) -> Work {
                     auto v = std::make_shared<std::vector<int>>(randomInts(n, 1 << 24));
                     return [v] {
                         introalgo::VanEmdeBoas t(1 << 24);
                         for (int x : *v) t.insert(x);
                         for (int x : *v) sink = sink + t.successor(x);
                     };
                 }});
    b.push_back({"Union-find: n random unions", "~O(n)", 1.0, doubling(1 << 17, 5), doubling(1 << 10, 3), [](int n) -> Work {
                     auto pairs = std::make_shared<std::vector<int>>(randomInts(2 * n, n));
                     return [pairs, n] {
                         introalgo::UFSet uf(n);
                         for (std::size_t i = 0; i + 1 < pairs->size(); i += 2) uf.Union((*pairs)[i], (*pairs)[i + 1]);
                         sink = sink + uf.components();
                     };
                 }});
    b.push_back({"LUP decomposition (n x n)", "O(n^3)", 3.0, doubling(50, 4), doubling(8, 3), [](int n) -> Work {
                     auto m = std::make_shared<introalgo::Matrix>(static_cast<std::size_t>(n), std::vector<double>(static_cast<std::size_t>(n)));
                     for (auto& row : *m)
                         for (auto& x : row) x = static_cast<double>(rng() % 1000) / 100.0 - 5.0;
                     return [m] { sink = sink + static_cast<long long>(introalgo::LUP(*m).determinant() != 0.0); };
                 }});
    b.push_back({"All-pairs shortest paths (multmat)", "O(V^3 log V)", 3.0, doubling(20, 4), doubling(5, 3), [](int n) -> Work {
                     auto g = std::make_shared<introalgo::Graph>(n);
                     for (int i = 0; i < n * 4; ++i) {
                         const int u = static_cast<int>(rng() % static_cast<unsigned>(n)), v = static_cast<int>(rng() % static_cast<unsigned>(n));
                         if (u != v) g->addEdge(u, v, 1 + static_cast<int>(rng() % 9));
                     }
                     return [g] { sink = sink + g->multmat()[0][0]; };
                 }});
    b.push_back({"Max flow (Edmonds-Karp, sparse random)", "O(V E^2) worst case", std::nan(""), doubling(250, 5), doubling(20, 3), [](int n) -> Work {
                     auto g = std::make_shared<introalgo::FlowGraph>(n);
                     for (int i = 0; i < n * 4; ++i) {
                         const int u = static_cast<int>(rng() % static_cast<unsigned>(n)), v = static_cast<int>(rng() % static_cast<unsigned>(n));
                         g->addEdge(u, v, 1 + static_cast<long long>(rng() % 20));
                     }
                     return [g, n] { sink = sink + g->maxFlow(0, n - 1); };
                 }});
    b.push_back({"Simplex (n x n dense LP)", "polynomial in practice", std::nan(""), doubling(10, 4), doubling(4, 3), [](int n) -> Work {
                     auto A = std::make_shared<std::vector<std::vector<double>>>(static_cast<std::size_t>(n), std::vector<double>(static_cast<std::size_t>(n)));
                     auto bb = std::make_shared<std::vector<double>>(static_cast<std::size_t>(n), 0.0);
                     auto c = std::make_shared<std::vector<double>>(static_cast<std::size_t>(n), 0.0);
                     for (int i = 0; i < n; ++i) {
                         for (int j = 0; j < n; ++j) (*A)[static_cast<std::size_t>(i)][static_cast<std::size_t>(j)] = 1.0 + static_cast<double>(rng() % 100) / 10.0;
                         (*bb)[static_cast<std::size_t>(i)] = 100.0 + static_cast<double>(rng() % 100);
                         (*c)[static_cast<std::size_t>(i)] = 1.0 + static_cast<double>(rng() % 50) / 10.0;
                     }
                     return [A, bb, c] { sink = sink + static_cast<long long>(introalgo::Simplex(*A, *bb, *c).solve().objective); };
                 }});
    return b;
}

}  // namespace

int main(int argc, char** argv) {
    bool quick = false, csv = false;
    for (int i = 1; i < argc; ++i) {
        if (!std::strcmp(argv[i], "--quick")) quick = true;
        else if (!std::strcmp(argv[i], "--csv")) csv = true;
        else {
            std::fprintf(stderr, "usage: %s [--quick] [--csv]\n", argv[0]);
            return 2;
        }
    }
    if (csv) std::printf("algorithm,expected,n,seconds\n");
    else std::printf("%-42s %-24s %12s %12s %9s %9s\n", "algorithm", "expected", "largest n", "time (ms)", "fit k", "textbook");

    for (const Bench& bench : makeBenches()) {
        const std::vector<int>& sizes = quick ? bench.quickSizes : bench.sizes;
        std::vector<double> times;
        for (int n : sizes) {
            const auto work = bench.setup(n);
            times.push_back(seconds(work));
            if (csv) std::printf("\"%s\",\"%s\",%d,%.6f\n", bench.name.c_str(), bench.expected.c_str(), n, times.back());
        }
        if (!csv) {
            char reference[16] = "-";
            if (!std::isnan(bench.expectedExponent)) std::snprintf(reference, sizeof reference, "%.1f", bench.expectedExponent);
            std::printf("%-42s %-24s %12d %12.2f %9.2f %9s\n", bench.name.c_str(), bench.expected.c_str(), sizes.back(),
                        times.back() * 1000.0, fitExponent(sizes, times), reference);
        }
    }
    return sink == -1 ? 1 : 0;  // never true; uses `sink` so the work cannot be optimised away
}
