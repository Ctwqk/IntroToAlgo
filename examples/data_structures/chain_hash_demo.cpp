// The original chain_hash.cpp had an empty main(); this shows chaining and collisions.
#include <iostream>

#include "introalgo/data_structures/chain_hash.hpp"

int main() {
    introalgo::ChainHash h;  // 10 buckets
    for (int v : {5, 15, 25, 7, 42, 17}) h.insert(v);
    for (int b = 0; b < h.bucketCount(); ++b) {
        if (h.bucket(b).empty()) continue;
        std::cout << "bucket " << b << ":";
        for (int v : h.bucket(b)) std::cout << " " << v;
        std::cout << std::endl;
    }
    h.delete_val(15);
    std::cout << "after deleting 15, bucket 5:";
    for (int v : h.bucket(5)) std::cout << " " << v;
    std::cout << std::endl;
}
