// Assignment 6 (the original root-level assignment6.cpp): spam detection with a trie and DNA
// matching with Rabin-Karp.  The classes now come from the library (introalgo::Trie and
// introalgo::DNAMatch); this file keeps the original driver code.
#include <iostream>
#include <string>
#include <vector>

#include "introalgo/strings/rolling_hash.hpp"
#include "introalgo/strings/trie.hpp"

using namespace std;
using introalgo::DNAMatch;
using introalgo::Trie;

void detectSpam() {
    vector<string> K{"asda",     "aebrq", "bqrqwf",  "VASVE",   "vasdawdf", "dasegawggdav",
                     "efwd",     "asd",   "davcase", "vwdfd",   "davsef"};
    string message = "I am asda, u r asd, please bqrqwf to VASVE, so that u efwd a dasegawggdav";
    vector<string> wordInMessage;
    while (true) {
        size_t pos = message.find(' ');
        if (pos == string::npos) break;
        wordInMessage.push_back(message.substr(0, pos));
        message = message.substr(pos + 1, message.size() - pos);
        string s = wordInMessage.back();
        char c = s.back();
        if (c < 65 || (c > 90 && c < 97) || c > 122) wordInMessage.back() = s.substr(0, s.size() - 1);
    }
    wordInMessage.push_back(message);

    Trie root;
    root.inserts(K);
    cout << "In Trie: " << endl;
    root.show(cout);
    for (const string& s : wordInMessage) {
        if (root.search(s)) {
            cout << "detected: " << s << endl;
        }
    }
}

/********below is problem 4**********/

void testDNAMatch() {
    string target =
        "TATTACTAGCTACTGTACTCGGCAGCCTATTCACTAGCTACTGTACTGGAGTACTATTCGGTGCGTATTCGACTAGCTACTGTACTGATAACGTACGT";
    string pattern = "ACTAGCTACTGTACT";
    DNAMatch dnam(pattern, target);
    cout << "target: " << target << endl;
    cout << "pattern: " << pattern << endl;
    cout << "Pattern appears in following positions: ";
    for (int i : dnam.match()) cout << i << " ";
    cout << endl;
}

int main() {
    cout << "SpamDetect: " << endl;
    detectSpam();
    cout << endl;
    cout << "DNAMatch: " << endl;
    testDNAMatch();
}
