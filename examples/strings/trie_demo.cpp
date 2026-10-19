// Original demo of trie.cpp (detectSpam): flag the words of a message that are in a keyword trie.
#include <iostream>
#include <string>
#include <vector>

#include "introalgo/strings/trie.hpp"

int main() {
    std::vector<std::string> K{"asda", "aebrq", "bqrqwf",  "VASVE", "vasdawdf", "dasegawggdav",
                               "efwd", "asd",   "davcase", "vwdfd", "davsef"};
    std::string message =
        "I am asda, u r asd, please bqrqwf to VASAE, so that u efwd a dasegawggdav";
    std::vector<std::string> wordInMessage;
    while (true) {
        std::size_t pos = message.find(' ');
        if (pos == std::string::npos) break;
        wordInMessage.push_back(message.substr(0, pos));
        message = message.substr(pos + 1);
        std::string s = wordInMessage.back();
        char c = s.back();
        if (c < 65 || (c > 90 && c < 97) || c > 122)
            wordInMessage.back() = s.substr(0, s.size() - 1);
    }
    wordInMessage.push_back(message);

    introalgo::Trie root;
    root.inserts(K);
    for (const std::string& s : wordInMessage) {
        if (root.search(s)) std::cout << "detected: " << s << std::endl;
    }
}
