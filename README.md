# IntroToAlgo

A personal repository of classic algorithms and data-structure implementations in C++.

This repo is organized as a topic-based practice collection rather than a single application. It includes implementations and exercises across dynamic programming, graph algorithms, string algorithms, sorting, hashing, matrix methods, balanced trees, backtracking, and more.

## Covered Topics

- Dynamic programming (`LCS`, `editDistance`, `OBST`, longest path, etc.)
- String processing (`KMP`, rolling hash, trie)
- Graph algorithms and flows
- Sorting and heaps
- Hash tables
- Trees / BST variants
- Backtracking and assignment-style problems
- Linear programming / simplex experimentation

## Repository Layout

```text
DandC/                # Divide-and-conquer exercises
Dynamic_Programing/   # DP and string-related implementations
Greedt/               # Greedy algorithm experiments
LinearPrograming/     # Simplex / LP practice
graph/                # Graph and flow algorithms
sort/                 # Sorting implementations
string/               # String data structures and hashing
heap/                 # Heap implementations
hash/                 # Hash-table exercises
bst_bbst/             # Tree structures
backtrack/            # Backtracking problems
matrix/               # Matrix / decomposition work
```

## Getting Started

Most files are standalone C++ programs and can be compiled individually:

```bash
g++ -std=c++17 Dynamic_Programing/LCS.cpp -o lcs
./lcs
```

For subdirectories that already include a `CMakeLists.txt`, you can also build them with CMake:

```bash
cmake -S Dynamic_Programing -B build
cmake --build build
```

## Notes

- The directory names and organization reflect coursework / personal practice history.
- This repository is best used as a reference shelf of algorithm implementations rather than a library package.
