#include <gtest/gtest.h>

#include <algorithm>
#include <cctype>
#include <set>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "introalgo/strings/trie.hpp"
#include "test_utils.hpp"

using introalgo::Trie;

namespace {

std::string lower(std::string s) {
    for (auto& c : s) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return s;
}

}  // namespace

TEST(Trie, InsertSearchDelete) {
    Trie t;
    EXPECT_TRUE(t.inserts({"asda", "asd", "bqrqwf"}));
    EXPECT_EQ(t.size(), 3u);
    EXPECT_TRUE(t.search("asd"));
    EXPECT_TRUE(t.search("asda"));
    EXPECT_FALSE(t.search("as"));  // a prefix of a word is not a word
    EXPECT_FALSE(t.search("asdaa"));
    EXPECT_FALSE(t.search("zzz"));

    EXPECT_TRUE(t.del("asd"));
    EXPECT_FALSE(t.search("asd"));
    EXPECT_TRUE(t.search("asda"));  // the longer word survives
    EXPECT_FALSE(t.del("asd"));     // already gone
    EXPECT_TRUE(t.del("asda"));
    EXPECT_EQ(t.size(), 1u);
    EXPECT_TRUE(t.consistent());
}

TEST(Trie, LettersAreCaseInsensitive) {
    Trie t;
    t.insert("VASVE");
    EXPECT_TRUE(t.search("vasve"));
    EXPECT_TRUE(t.search("VASVE"));
    EXPECT_EQ(t.words(), (std::vector<std::string>{"vasve"}));
}

TEST(Trie, InvalidCharactersAreRejectedWithoutSideEffects) {
    Trie t;
    t.insert("abc");
    // The original bumped the edge counters of the valid prefix before noticing the bad letter.
    EXPECT_FALSE(t.insert("abx1"));
    EXPECT_FALSE(t.insert("ab c"));
    EXPECT_FALSE(t.search("a1"));
    EXPECT_FALSE(t.del("abc!"));
    EXPECT_EQ(t.size(), 1u);
    EXPECT_TRUE(t.consistent());
    EXPECT_TRUE(t.del("abc"));
    EXPECT_EQ(t.size(), 0u);
}

TEST(Trie, DuplicateInsertsAndEmptyWord) {
    Trie t;
    EXPECT_TRUE(t.insert("dup"));
    EXPECT_TRUE(t.insert("dup"));
    EXPECT_EQ(t.size(), 1u);
    EXPECT_TRUE(t.del("dup"));
    EXPECT_FALSE(t.search("dup"));  // one delete is enough, no ghost copies
    EXPECT_TRUE(t.consistent());

    EXPECT_FALSE(t.search(""));
    EXPECT_TRUE(t.insert(""));
    EXPECT_TRUE(t.search(""));
    EXPECT_TRUE(t.del(""));
    EXPECT_FALSE(t.search(""));
}

TEST(Trie, ChangeAndBatchHelpers) {
    Trie t;
    t.inserts({"cat", "car"});
    EXPECT_TRUE(t.change("cat", "dog"));
    EXPECT_FALSE(t.search("cat"));
    EXPECT_TRUE(t.search("dog"));
    EXPECT_FALSE(t.change("cow", "x"));      // source missing
    EXPECT_FALSE(t.change("car", "no-no"));  // invalid target must not lose "car"
    EXPECT_TRUE(t.search("car"));
    EXPECT_EQ(t.searchs({"car", "cat", "dog"}), (std::vector<bool>{true, false, true}));
    EXPECT_FALSE(t.dels({"car", "nothing"}));
    EXPECT_FALSE(t.inserts({"ok", "not ok"}));
    EXPECT_TRUE(t.search("ok"));
}

TEST(Trie, ShowPrintsWordsInLexicographicOrderAndCanBeRepeated) {
    // The original marked nodes as visited and could print the trie only once.
    Trie t;
    t.inserts({"asda", "aebrq", "asd", "bqrqwf"});
    std::ostringstream first, second;
    t.show(first);
    t.show(second);
    EXPECT_EQ(first.str(), "aebrq\nasd\nasda\nbqrqwf\n");
    EXPECT_EQ(first.str(), second.str());
}

TEST(Trie, MoveTransfersOwnership) {
    Trie a;
    a.insert("moved");
    Trie b = std::move(a);
    EXPECT_TRUE(b.search("moved"));
}

TEST(Trie, RandomizedOperationsMatchStdSet) {
    testutil::Rng rng(15);
    Trie t;
    std::set<std::string> oracle;
    const std::string alphabet = "abAB";  // tiny alphabet: lots of shared prefixes
    for (int step = 0; step < 6000; ++step) {
        const std::string w =
            testutil::randString(rng, std::size_t(testutil::randInt(rng, 0, 5)), alphabet);
        switch (testutil::randInt(rng, 0, 2)) {
            case 0:
                EXPECT_TRUE(t.insert(w));
                oracle.insert(lower(w));
                break;
            case 1:
                EXPECT_EQ(t.del(w), oracle.erase(lower(w)) == 1);
                break;
            default:
                EXPECT_EQ(t.search(w), oracle.count(lower(w)) == 1);
        }
        if (step % 50 == 0) {
            EXPECT_TRUE(t.consistent());
            EXPECT_EQ(t.words(), std::vector<std::string>(oracle.begin(), oracle.end()));
            EXPECT_EQ(t.size(), oracle.size());
        }
    }
    EXPECT_TRUE(t.consistent());
}

TEST(Trie, MovedFromTriesRemainUsable) {
    Trie source;
    source.inserts({"", "alpha", "alps"});
    Trie moved(std::move(source));
    EXPECT_EQ(moved.words(), (std::vector<std::string>{"", "alpha", "alps"}));
    EXPECT_EQ(source.size(), 0u);
    EXPECT_FALSE(source.search(""));
    EXPECT_FALSE(source.del("alpha"));
    EXPECT_TRUE(source.words().empty());
    EXPECT_TRUE(source.consistent());
    EXPECT_TRUE(source.insert("new"));
    source = std::move(moved);
    EXPECT_EQ(source.size(), 3u);
    EXPECT_TRUE(source.consistent());
    EXPECT_TRUE(moved.insert("reused"));
    EXPECT_EQ(moved.size(), 1u);
    EXPECT_TRUE(moved.consistent());
    Trie& alias = source;
    source = std::move(alias);
    EXPECT_EQ(source.size(), 3u);
    EXPECT_TRUE(source.consistent());
}
