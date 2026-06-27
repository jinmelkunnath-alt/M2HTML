#!/bin/sh
set -eu
make static-analysis
if command -v clang-tidy >/dev/null 2>&1; then
  find src -name '*.c' -print | while read f; do clang-tidy "$f" -- -std=c99 -Iinclude; done
else
  echo "clang-tidy not installed; compiler static analysis completed"
fi
if command -v cppcheck >/dev/null 2>&1; then
  cppcheck --enable=warning,style,performance,portability --std=c99 -Iinclude src include tests
else
  echo "cppcheck not installed; skipping cppcheck"
fi
