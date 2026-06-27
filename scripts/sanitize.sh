#!/bin/sh
set -eu
cc=${CC:-cc}
flags="-std=c99 -Wall -Wextra -Wpedantic -g -O1 -fsanitize=address,undefined -fno-omit-frame-pointer -Iinclude"
rm -f build/sanitize_tests
mkdir -p build
$cc $flags src/*.c tests/*.c -o build/sanitize_tests
ASAN_OPTIONS=detect_leaks=1 build/sanitize_tests
