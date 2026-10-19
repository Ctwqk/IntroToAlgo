#include "introalgo/data_structures/chain_hash.hpp"

#include <stdexcept>
#include <utility>

namespace introalgo {

ChainHash::ChainHash(int buckets) {
    if (buckets <= 0) throw std::invalid_argument("ChainHash: need at least one bucket");
    map.assign(static_cast<std::size_t>(buckets), nullptr);
}

ChainHash::~ChainHash() {
    clear();
}

ChainHash::ChainHash(const ChainHash& other) : map(other.map.size(), nullptr) {
    for (std::size_t i = 0; i < other.map.size(); i++) {
        ListNode** tail = &map[i];
        for (const ListNode* p = other.map[i]; p; p = p->next) {
            *tail = new ListNode(p->val);
            tail = &(*tail)->next;
        }
    }
    count = other.count;
}

ChainHash& ChainHash::operator=(ChainHash other) {
    std::swap(map, other.map);
    std::swap(count, other.count);
    return *this;
}

ChainHash::ChainHash(ChainHash&& other) noexcept : map(std::move(other.map)), count(other.count) {
    other.map.clear();
    other.count = 0;
}

void ChainHash::clear() {
    for (ListNode*& head : map) {
        while (head) {
            ListNode* next = head->next;
            delete head;
            head = next;
        }
    }
    count = 0;
}

int ChainHash::Hash_func(int key1) const {
    const long long size = static_cast<long long>(map.size());
    return static_cast<int>(((key1 % size) + size) % size);
}

void ChainHash::insert(int val) {
    int k = Hash_func(val);
    if (map[k] == nullptr) {
        map[k] = new ListNode(val);
    } else {
        ListNode* p = map[k];
        while (p->next) p = p->next;
        p->next = new ListNode(val);
    }
    count++;
}

bool ChainHash::delete_val(int val) {
    // Unlink the first node holding `val`.  (The original walked to the end of the chain and
    // assigned nullptr to a local variable, so nothing was ever removed.)
    ListNode** link = &map[Hash_func(val)];
    while (*link && (*link)->val != val) link = &(*link)->next;
    if (!*link) return false;
    ListNode* victim = *link;
    *link = victim->next;
    delete victim;
    count--;
    return true;
}

const ListNode* ChainHash::search(int v) const {
    const ListNode* p = map[Hash_func(v)];
    while (p) {
        if (p->val == v) return p;
        p = p->next;
    }
    return nullptr;
}

bool ChainHash::modify(int t, int v) {
    // The new value may hash to another bucket, so it is a delete followed by an insert.
    if (!delete_val(t)) return false;
    insert(v);
    return true;
}

void ChainHash::sortedInsert(int v, const std::string& ord) {
    const bool increasing = ord == "increasing";
    if (!increasing && ord != "decreasing")
        throw std::invalid_argument(
            "ChainHash::sortedInsert: order must be increasing or decreasing");
    // Walk past every node that belongs before the new value (equal values stay in insertion
    // order).
    ListNode** link = &map[Hash_func(v)];
    while (*link && (increasing ? (*link)->val <= v : (*link)->val >= v)) link = &(*link)->next;
    *link = new ListNode(v, *link);
    count++;
}

std::vector<int> ChainHash::bucket(int index) const {
    std::vector<int> out;
    for (const ListNode* p = map.at(static_cast<std::size_t>(index)); p; p = p->next)
        out.push_back(p->val);
    return out;
}

}  // namespace introalgo
