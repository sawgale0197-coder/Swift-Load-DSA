#!/usr/bin/env sh
set -eu

SCRIPT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
cd "$SCRIPT_DIR"

CXX=${CXX:-clang++}
OUTPUT=${OUTPUT:-main}

echo "Compiling main.cpp..."
"$CXX" -std=c++17 -Wall -Wextra -pedantic main.cpp -o "$OUTPUT"

echo "Running ./$OUTPUT"
"./$OUTPUT"
