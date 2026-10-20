# Verification record

Reviewed on 2026-10-19. The takeover started at `f9226ab`; the implementation verified below is at `cd7709f`.
Toolchains: GCC 14.2.0, Clang 19, CMake 3.31.6 and clang-format 23.1.3 on Linux (Intel i9-12900H).

All local build/test/benchmark steps ran with at most four build workers, the shared `flock` lock, a systemd
user scope with `MemoryMax=6G` and `MemorySwapMax=0`, and a check for at least 8 GiB available before starting.
Independent scratch directories were created under `/tmp/pu-introtoalgo-review-YnDHuw`.

## Release and sanitizer builds

The following commands used `verify=/tmp/pu-introtoalgo-review-YnDHuw` (each block inside the resource wrapper):

```bash
cmake -S . -B "$verify/pu-introtoalgo-gcc" -DINTROALGO_WERROR=ON
cmake --build "$verify/pu-introtoalgo-gcc" -j4
ctest --test-dir "$verify/pu-introtoalgo-gcc" --output-on-failure
```

Result: **229/229 passed**, 3.48 seconds: 202 GoogleTest cases, 26 example programs and one benchmark smoke test.

```bash
cmake -S . -B "$verify/pu-introtoalgo-asan" -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_CXX_COMPILER=clang++-19 -DINTROALGO_SANITIZE=ON -DINTROALGO_WERROR=ON
cmake --build "$verify/pu-introtoalgo-asan" -j4
ASAN_OPTIONS=detect_leaks=1:strict_string_checks=1 \
UBSAN_OPTIONS=print_stacktrace=1:halt_on_error=1 \
  ctest --test-dir "$verify/pu-introtoalgo-asan" --output-on-failure
```

Result: **229/229 passed**, 57.71 seconds, with no AddressSanitizer, LeakSanitizer or UBSan reports.

## Regression evidence

A small driver compiled against the saved `f9226ab` sources with
`g++ -std=c++17 -g -fsanitize=undefined -fno-sanitize-recover=undefined` reproduced:

- `ChainHash`: division by zero after moving from the object and searching the source.
- `Trie`: null-node access after moving from the object and searching the source.
- `VanEmdeBoas`: a reference bound through an empty cluster array after a move.
- `Knapsack`: signed overflow for item values `INT_MAX` and 1 at capacity 2.
- `RodCutting`: signed overflow for prices `{INT_MAX, 0}` at length 2.
- `Expression`: signed overflow for `999999999999999*999999999999999`.
- `FlowGraph`: signed overflow for parallel capacities `LLONG_MAX` and 1.

All corresponding regression tests pass in both builds above. The `INT_MIN` rod-cutting reconstruction defect
was identified by inspection and covered by a terminating regression test. Details and the original coursework
bugs are listed in [PORTING_NOTES.md](PORTING_NOTES.md).

## Formatting and repository checks

```bash
git ls-files '*.cpp' '*.hpp' | xargs clang-format --dry-run --Werror
git diff --check
```

Both passed. No binary contents or common credential patterns were found in tracked files. The repository
GitHub description and eight topics were set and read back successfully.

## GitHub Actions

[Implementation run 39343188369](https://github.com/Ctwqk/IntroToAlgo/actions/runs/39343188369) for `cd7709f`:
**success**, all five jobs passed (GCC/Clang Release with package consumers, GCC/Clang ASan+UBSan, formatting).


## Clean checkout, examples and library consumers

A local clone with no pre-existing build files was created from the tested branch:

```bash
git clone --no-hardlinks --branch portfolio-upgrade \
  /home/taiwei/portfolio-upgrade/IntroToAlgo "$verify/pu-introtoalgo-clean"
cd "$verify/pu-introtoalgo-clean"
cmake -S . -B build
cmake --build build -j4
ctest --test-dir build --output-on-failure
./build/examples/lcs_demo
./scripts/run_examples.sh build > ../examples-output.txt
./build/benchmarks/scaling > ../benchmark-results.txt
```

Result: **229/229 passed**, 3.50 seconds. All 26 examples completed, and their complete output matched
[examples-output.txt](examples-output.txt) byte for byte. The full 17-algorithm benchmark completed; its new
capture is [benchmark-results.txt](benchmark-results.txt), with selected rows in the README.

```bash
cmake --install build --prefix "$verify/pu-introtoalgo-install"
cmake -S tests/package_consumer -B "$verify/pu-introtoalgo-consumer" \
  -DCMAKE_PREFIX_PATH="$verify/pu-introtoalgo-install"
cmake --build "$verify/pu-introtoalgo-consumer" -j4
"$verify/pu-introtoalgo-consumer/use_introalgo"
```

Result: `introalgo package works: yes`. A separate parent project using `add_subdirectory` also built and ran
the consumer, with examples/tests/benchmarks correctly disabled by default. Each of the 27 public headers
compiled in isolation with `g++ -std=c++17 -Wall -Wextra -Wpedantic -Werror -Iinclude -x c++ -fsyntax-only`.

For the LCS allocation regression, a standalone driver evaluated
`Lcs(std::string(8000, 'a'), std::string(8000, 'a')).getLCSLength()` under a 64 MiB virtual-memory limit:

```bash
(ulimit -v 65536; "$verify/lcs-before") # saved f9226ab source: caught std::bad_alloc, exit 42
(ulimit -v 65536; "$verify/lcs-after")  # cd7709f library: printed 8000, exit 0
```

The baseline failure was caught by the driver; no out-of-memory kill was needed. The README's `ABCBDAB` / `BDCABA`
sequence example was also run and returned `BDAB` as shown.

## Independent review of `5368db0`

The independent review started from the pushed branch at `5368db0`. Its existing
[CI run 39343756800](https://github.com/Ctwqk/IntroToAlgo/actions/runs/39343756800) passed all five jobs.
The reviewer used a new `/tmp/pu-review-introtoalgo-MUFss5` scratch directory. Every local compilation,
test and benchmark run used the shared lock and a systemd user scope with `MemoryMax=6G` and
`MemorySwapMax=0`; available memory was checked before requesting the lock and again after acquiring it.
Build parallelism was at most four. No containers or system development packages were needed.

A fresh HTTPS clone of `portfolio-upgrade` ran the README commands unchanged:

```bash
cmake -S . -B build
cmake --build build -j4
ctest --test-dir build --output-on-failure
./build/examples/lcs_demo
./scripts/run_examples.sh build
./build/benchmarks/scaling
```

Baseline result: **229/229 tests passed** (3.40 seconds); all 26 example outputs matched the committed
capture byte for byte. The full 17-algorithm benchmark completed, and the installed-package consumer
printed `introalgo package works: yes`.

Before fixing the newly found issues, a targeted driver built the affected test/source files with
`-fsanitize=undefined -fno-sanitize-recover=undefined`. It reproduced:

- `Sort.RadixSortRejectsNegativeDigitCounts`: no exception for -1, followed by UBSan reporting signed
  overflow in `INT_MIN - 1`.
- `OptimalStopping.EdgeCases`: a single candidate's success probability was 0 instead of 1.
- `Graph.ExhaustiveMstOracleHandlesNegativeTotals`: oracle result 0 instead of the hand-checked -9.
- `Lup.ResidualOracleRejectsNonFiniteValues`: NaN differences, including infinity minus infinity,
  were reported as zero residual.

The review also checked the simplex vertex-enumeration and flow minimum-cut oracles, graph negative-weight
handling, LUP singularity checks, and the original LUP/OBST/LCS/sorting sources. Those implementations retain
their original structure; unfinished additions are identified in the porting notes. A focused read-only
review of the fixes found no blocking issue and led to an explicit edge-existence check in the forest validator.

The corrected sources were then built in separate scratch build directories:

```bash
cmake -S . -B "$review/pu-review-introtoalgo-gcc" -DINTROALGO_WERROR=ON
cmake --build "$review/pu-review-introtoalgo-gcc" -j4
ctest --test-dir "$review/pu-review-introtoalgo-gcc" --output-on-failure

cmake -S . -B "$review/pu-review-introtoalgo-sanitize" -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_CXX_COMPILER=clang++-19 -DINTROALGO_SANITIZE=ON -DINTROALGO_WERROR=ON
cmake --build "$review/pu-review-introtoalgo-sanitize" -j4
ASAN_OPTIONS=detect_leaks=1:strict_string_checks=1 \
UBSAN_OPTIONS=print_stacktrace=1:halt_on_error=1 \
  ctest --test-dir "$review/pu-review-introtoalgo-sanitize" --output-on-failure
```

Here `review=/tmp/pu-review-introtoalgo-MUFss5`, and each block was run inside the resource wrapper described
above. GCC 14.2.0 Release passed **232/232 tests** in 3.55 seconds (205 unit cases, 26 examples and one benchmark
smoke test). The corrected examples still matched the committed output. Both installed-package and
`add_subdirectory` consumers built and ran; the latter also checked that examples/tests/benchmarks were disabled.

Clang 19.1.7 with ASan/UBSan passed **232/232 tests** in 57.91 seconds with no sanitizer reports.
The verified code/test fixes are `aed2088` (radix input validation), `949ed09` (optimal stopping), and
`8344575` (MST/LUP oracles). Formatting with clang-format 23.1.3 and `git diff --check` passed.
The tracked-file scan found no binary contents, files over 1 MB, or common private-key/token patterns;
runtime sources contained no hard-coded host paths. Scratch artifacts were removed after verification.
