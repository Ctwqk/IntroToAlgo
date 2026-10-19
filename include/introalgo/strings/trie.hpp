#pragma once

#include <array>
#include <iosfwd>
#include <memory>
#include <string>
#include <vector>

namespace introalgo {

/// A trie over the letters a-z.  Upper and lower case are the same letter (words are stored
/// lower-case), every other character is rejected.
///
/// Like the original TrieNode, every edge carries a counter ("label") of the words that pass
/// through it; an edge whose counter drops to zero is pruned on deletion.  Words form a set:
/// inserting a word twice is the same as inserting it once.
///
/// All operations take O(length of the word).
class Trie {
public:
    Trie();
    ~Trie();
    /// Transfers ownership; the source becomes an empty, reusable trie.
    Trie(Trie&&);
    Trie& operator=(Trie&&);
    Trie(const Trie&) = delete;
    Trie& operator=(const Trie&) = delete;

    /// Inserts a word.  Returns false (and leaves the trie unchanged) if it contains a character
    /// other than a letter.  The empty word is allowed.
    bool insert(const std::string& word);
    /// Inserts every word; true only if all of them were valid (valid ones are inserted anyway).
    bool inserts(const std::vector<std::string>& words);

    /// Removes a word; false if it was not in the trie.
    bool del(const std::string& word);
    /// Removes every word; true only if all of them were present.
    bool dels(const std::vector<std::string>& words);

    bool search(const std::string& s) const;
    std::vector<bool> searchs(const std::vector<std::string>& strings) const;

    /// Replaces word `s` by `tars`; fails (changing nothing) if `s` is absent or `tars` is invalid.
    bool change(const std::string& s, const std::string& tars);

    /// All words in lexicographic order.
    std::vector<std::string> words() const;
    /// Prints all words, one per line (the original printed them while unwinding its DFS).
    void show(std::ostream& os) const;

    /// Number of distinct words.
    std::size_t size() const { return count; }

    /// Checks the internal counters (every edge label equals the number of words below it).
    /// Meant for tests.
    bool consistent() const;

private:
    struct Node {
        std::array<std::unique_ptr<Node>, 26> children;
        std::array<int, 26> labels{};
        bool isEnd = false;
    };
    std::unique_ptr<Node> root;
    std::size_t count = 0;

    static int judge(char c);  // letter -> 0..25, or -1
    static char decode(int val);
    static bool valid(const std::string& s);
};

}  // namespace introalgo
