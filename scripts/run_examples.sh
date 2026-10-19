#!/usr/bin/env bash
# Runs every example executable of an existing build and prints their output, section by section.
# Usage: scripts/run_examples.sh [build-dir]    (default: build)
set -euo pipefail
build="${1:-build}"
for exe in "$build"/examples/*; do
    [ -x "$exe" ] || continue
    printf '===== %s =====\n' "$(basename "$exe")"
    "$exe"
    printf '\n'
done
