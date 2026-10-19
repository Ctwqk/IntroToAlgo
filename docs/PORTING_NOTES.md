# Porting notes

What happened to each file of the original repository (uploaded as one commit in February 2025), and which bugs
the new tests found. The original sources are still in the git history (`git show main:<path>`).

How to read the "Evidence" column:

- **reproduced** - I ran the original code next to the corrected version (or an oracle) and it gave a wrong result
  or crashed; the numbers are quoted.
- **compile error** - the original file does not compile as it was committed (g++ 14, `-std=c++17`).
- **by inspection** - found while reading the code; the regression test shows the corrected behaviour.

## Where every original file went

| Original path | Now | Notes |
|---|---|---|
| `DandC/OutOfRanking.cpp` | `src/divide_and_conquer/out_of_ranking.cpp` | `OutOfRanking` |
| `Dynamic_Programing/LCS.cpp` | `src/dynamic_programming/lcs.cpp` | `lcs` -> `Lcs` |
| `Dynamic_Programing/editDistance.cpp` | `src/dynamic_programming/edit_distance.cpp` | `ED` (no `main` in the original) |
| `Dynamic_Programing/LMIS.cpp` | `src/dynamic_programming/lmis.cpp` | `lmis` -> `Lmis` |
| `Dynamic_Programing/OBST.cpp` | `src/dynamic_programming/obst.cpp` | `obst` -> `Obst`; algorithm unchanged, the tree printing is kept word for word |
| `Dynamic_Programing/LongSinglePath.cpp` | `src/dynamic_programming/bitonic_tour.cpp` | the file name was misleading: it computes the shortest *bitonic tour*; `lsp` -> `BitonicTour` |
| `Dynamic_Programing/MaxQuadExp.cpp` | `src/dynamic_programming/max_quad_exp.cpp` | `Expression`; a committed ELF binary `Dynamic_Programing/MaxQuadExp` was deleted |
| `Dynamic_Programing/KMP.cpp` | `src/strings/kmp.cpp` | `KMP` |
| `Greedt/knapsack_0_1.cpp` | `src/dynamic_programming/knapsack_01.cpp` | 0/1 knapsack is a DP, not a greedy algorithm; `knapsack` -> `Knapsack` |
| `Greedt/rogcutting.cpp` | `src/dynamic_programming/rod_cutting.cpp` | was an empty `cutting` class ("just begin"); implemented as `RodCutting` |
| `sy.cpp` | `src/greedy/solar_panels.cpp` | the only genuinely greedy program; `maximum_num_houses_solar` -> `MaximumNumHousesSolar` |
| `LinearPrograming/SimplexAlgo.cpp` | `src/linear_programming/simplex.cpp` | was a **zero-byte file**; the simplex method is new code, written for this port |
| `graph/graph.cpp` | `src/graph/graph.cpp` | `UFSet`, `graph` -> `Graph`; the methods that were only declared are implemented |
| `graph/Flow.cpp` | `src/graph/flow_graph.cpp` | unfinished sketch, completed as `FlowGraph` (Edmonds-Karp) |
| `heap/max_heap.cpp` | `src/data_structures/max_heap.cpp` | `Heap`; `dat` -> `HeapEntry` |
| `hash/chain_hash.cpp` | `src/data_structures/chain_hash.cpp` | `myHash` -> `ChainHash` |
| `bst_bbst/bst.cpp` | `src/data_structures/bst.cpp`, `rb_tree.cpp` | `TreeNode` + new owning `Bst`; the unfinished `BbstNode`/`RbNode` became a complete `RbTree` |
| `vBE/vBEstructure.cpp` | `src/data_structures/van_emde_boas.cpp` | `vBE` -> `VanEmdeBoas` ("not tested, just used to understand the vEB" in the original) |
| `sort/sort.cpp` | `src/sorting/sort.cpp` | `Sort`; `dat` -> `DigitKey` |
| `string/RollingHash.cpp` | `src/strings/rolling_hash.cpp` | `DNAMatch` |
| `string/trie.cpp` | `src/strings/trie.cpp` | `TrieNode` -> `Trie` |
| `assignment6.cpp` | `examples/assignment6_spam_and_dna.cpp` | duplicated copies of the trie and Rabin-Karp classes replaced by the library; the driver code is unchanged |
| `backtrack/numbers.cpp` | `src/backtracking/constrained_permutations.cpp` | `numbers` -> `ConstrainedPermutations` |
| `matrix/LUP.cpp` | `src/matrix/lup.cpp` | `LUP`, `SolveEquation` |
| `notype/beststop.cpp` | `src/math/optimal_stopping.cpp` | `CalFormula` (the secretary problem) |
| `projects/spinning.cpp` | `src/geometry/rotating_cube.cpp` | `cube` -> `Cube` |
| `dfs.cpp`, `assignment4/problem1.cpp` | deleted | contained only `#include`s / an empty class, no algorithm |
| `Dynamic_Programing/CMakeLists.txt` | deleted | CMake 2.8, one target; replaced by the top-level `CMakeLists.txt` |

Directory names with typos (`Greedt`, `Dynamic_Programing`, `LinearPrograming`) were fixed by `git mv`, so
`git log --follow` still works for every file.

## Bugs found by the tests

### Programs that did not compile

| File | Problem |
|---|---|
| `graph/graph.cpp` | missing `#include <cstring>` for `memcpy` (compile error) |
| `graph/Flow.cpp` | syntax errors (`void setNum(int n) num=n;`), undefined names (`n`, `adj`, `vertives`), `private` in the middle of a class: did not compile and had no flow algorithm (compile error) |
| `hash/chain_hash.cpp` | default argument repeated on the out-of-class definition of `sortedInsert` (compile error) |
| `sy.cpp` | `INT_MAX` without `<climits>`, `isPrime` without a return value (compile error) |

### Wrong results or crashes

| Module | Bug | Evidence |
|---|---|---|
| `lcs` | the traceback indexed the table transposed (`tabulate[index1-1][index2]`) | **reproduced**: `getLCS()` segfaults on random non-square inputs |
| `edit_distance` | constructor initialised both strings from `s2`; the recurrence never compared characters | by inspection (with `min(...) + 1` everywhere the table reduces to `max(i, j)`, so every call returned the larger length, independent of the contents) |
| `lmis` | the running maximum was not reset for every position | **reproduced**: `1 2 3 0 1` gave 4, correct 3 |
| `knapsack_01` | an item that does not fit left a 0 in the table, patched afterwards by a max over rows | **reproduced**: items (10,1) (100,50) (20,1), capacity 3 gave 20, correct 30 |
| `max_quad_exp` | the minimum of a `-` node was computed as `-(maximum)`; no operator validation (an unknown operator printed a message and was evaluated as the value -1) | **reproduced**: 359 of 1000 random `+ - *` expressions had a wrong maximum, e.g. `3-7-8-3*2` gave 30, a value no parenthesization produces (correct 14) |
| `bitonic_tour` | the tour reconstruction was unfinished (`if(cop[j][i]=1)` assigns), and it overwrote the distance matrix with direction markers | by inspection; the DP value itself was right |
| `kmp` | after falling back through a shorter border the failure function was not extended by the matching character | **reproduced**: text `bbabbabbb`, pattern `babb` (2 of 3000 random small cases differed) |
| `rolling_hash` | `int` shift by `2m-2` bits overflows for patterns longer than 16 letters; the rolled hash was compared before the final reduction; `matches` accumulated across calls; a pattern longer than the text read out of bounds | **reproduced**: a 20-letter pattern copied out of a 40-letter text was not found at all |
| `trie` | `del()` fell off the end of a non-void function; `insert` bumped edge counters before validating; `search` indexed with -1 on bad letters; `show()` could run only once (visited flags never reset); `inserts` used `flag != insert(s)` | by inspection |
| `graph` | `UFSet::Union` linked the arguments instead of their representatives; the `(n, arr)` constructor copied `n` bytes instead of `n` ints; `showMat` looped to a hard-coded 11; `multmat` did one in-place pass instead of a closure | by inspection |
| `max_heap` | sift-up used the 1-based parent `pos/2` with 0-based children; `peek()` always returned a default element; fixed 100-slot array overflowed; `pop`/`peek`/`showHeap` on an empty heap were undefined | **reproduced**: heap property violated in 269 of 20000 random 2-7 element insert sequences (e.g. `5 9 15 7 6 3 15`) |
| `chain_hash` | `delete_val` removed nothing; table size ignored (always `% 10`); `delete map` instead of `delete[]` plus leaked nodes; `sortedInsert` used the value as the bucket index | by inspection |
| `bst` | `getDepth` had no return; `getPredecessor`/`getSuccessor`/`delNode` dereferenced null children; deleting the root left the caller with a dangling pointer; `RbNode::insert` had an infinite `while(true)`; `RbNode` shadowed the base class's pointers | by inspection |
| `van_emde_boas` | the constructor never allocated the clusters or the summary; index arithmetic used floating-point `log2`; `idx()` had no return; `del` used `==` instead of `=` | by inspection |
| `sort` | `dat` allocated `new int(n)` (one int, not an array) and then indexed it; counting sort read `arr[0]` of an empty array and overflowed `int` for wide ranges; quick sort recursed `n` deep on sorted input | by inspection |
| `lup` | `solve(int*)` truncated the right-hand side to integers; the caller's matrix was overwritten and its row pointers swapped; singular matrices divided by zero; `getReverse()` was empty | by inspection |
| `rotating_cube` | `static const int pi = M_PI` (= 3) in the degree conversion; degrees reduced modulo 180; row-vector multiplication rotated by the inverse angle; `show()` wrote into an uninitialised, too small canvas | by inspection |
| `out_of_ranking` | the counter was never reset; `int` counter overflows at n(n-1)/2 > 2^31; a leaked `new[]` per merge | by inspection |
| `constrained_permutations` | thresholds below 1 admitted a permutation; `n = 0` gave no answer; the `layer` counter leaked between `find()` calls | **reproduced**: `numbers(4, 0, 5)` returned 1 answer instead of none |

### Checked and found correct

`obst` (the DP and the printing), `solar_panels` (the greedy rule is optimal; checked against exhaustive
assignment and a max-flow oracle), `optimal_stopping` (matches exact enumeration of all arrival orders) and the
core recurrence of `constrained_permutations` (checked against all permutations for n <= 7 and every threshold
pair) needed no algorithmic changes.

## Behaviour changes worth knowing

- `Trie` is case-insensitive (as the original's `judge` already folded `A-Z` onto `a-z`) and lists words in
  lexicographic order (the original printed children before parents).
- `Expression` rejects malformed input (`-5+3`, `5 $ 3`) with `std::invalid_argument`; the original silently treated
  a leading `-` as `0 -` and printed a rude message for unknown operators.
- `Cube` rotates by the stated angle in the right-handed convention (`p -> R p`); the original rotated by the
  inverse angle.
- `Heap::sizee()` is now `size()`; `precessor` is now `predecessor`; the program-level printing moved to the
  examples.
