#include "introalgo/strings/trie.hpp"

#include <ostream>

namespace introalgo {

Trie::Trie() : root(new Node) {}
Trie::~Trie() = default;
Trie::Trie(Trie&&) noexcept = default;
Trie& Trie::operator=(Trie&&) noexcept = default;

char Trie::decode(int val) {
    if (val < 0 || val > 25) return '!';
    return static_cast<char>('a' + val);
}

// check whether an incoming letter is valid, a-z or A-Z
int Trie::judge(char c) {
    int pos = c - 'a';
    if (pos < 0 || pos > 25) pos = c - 'A';
    if (pos < 0 || pos > 25) return -1;
    return pos;
}

bool Trie::valid(const std::string& s) {
    for (char c : s)
        if (judge(c) == -1) return false;
    return true;
}

bool Trie::insert(const std::string& word) {
    if (!valid(word)) return false;  // nothing has been touched yet
    if (search(word)) return true;
    Node* p = root.get();
    for (char c : word) {
        const int pos = judge(c);
        if (++p->labels[pos] <= 1) p->children[pos].reset(new Node);
        p = p->children[pos].get();
    }
    p->isEnd = true;
    ++count;
    return true;
}

bool Trie::inserts(const std::vector<std::string>& strings) {
    bool flag = true;
    for (const auto& s : strings) flag &= insert(s);
    return flag;
}

bool Trie::del(const std::string& s) {
    if (!search(s)) return false;
    Node* p = root.get();
    for (char c : s) {
        const int pos = judge(c);
        Node* next = p->children[pos].get();
        if (--p->labels[pos] == 0) {
            // No other word uses this edge: the whole remaining path can go.
            p->children[pos].reset();
            --count;
            return true;
        }
        p = next;
    }
    p->isEnd = false;  // the word is a prefix of other words (or is the empty word)
    --count;
    return true;
}

bool Trie::dels(const std::vector<std::string>& strings) {
    bool flag = true;
    for (const auto& s : strings) flag &= del(s);
    return flag;
}

bool Trie::search(const std::string& s) const {
    const Node* p = root.get();
    for (char c : s) {
        const int pos = judge(c);
        if (pos == -1 || p->labels[pos] <= 0) return false;
        p = p->children[pos].get();
    }
    return p->isEnd;
}

std::vector<bool> Trie::searchs(const std::vector<std::string>& strings) const {
    std::vector<bool> ans;
    ans.reserve(strings.size());
    for (const auto& s : strings) ans.push_back(search(s));
    return ans;
}

bool Trie::change(const std::string& s, const std::string& tars) {
    if (!valid(tars) || !search(s)) return false;
    return del(s) && insert(tars);
}

std::vector<std::string> Trie::words() const {
    std::vector<std::string> out;
    std::string current;
    // Recursion depth is bounded by the length of the longest word.
    struct Walker {
        std::vector<std::string>& out;
        std::string& cur;
        void walk(const Node* p) {
            if (p->isEnd) out.push_back(cur);
            for (int i = 0; i < 26; i++) {
                if (p->labels[i] > 0) {
                    cur.push_back(decode(i));
                    walk(p->children[i].get());
                    cur.pop_back();
                }
            }
        }
    } w{out, current};
    w.walk(root.get());
    return out;
}

void Trie::show(std::ostream& os) const {
    for (const auto& w : words()) os << w << std::endl;
}

bool Trie::consistent() const {
    struct Checker {
        // Returns the number of words in the subtree, or -1 if some counter is wrong.
        static long long check(const Node* p) {
            long long total = p->isEnd ? 1 : 0;
            for (int i = 0; i < 26; i++) {
                if (p->labels[i] < 0) return -1;
                if ((p->labels[i] > 0) != (p->children[i] != nullptr)) return -1;
                if (p->labels[i] == 0) continue;
                const long long below = check(p->children[i].get());
                if (below != p->labels[i]) return -1;
                total += below;
            }
            return total;
        }
    };
    return Checker::check(root.get()) == static_cast<long long>(count);
}

}  // namespace introalgo
