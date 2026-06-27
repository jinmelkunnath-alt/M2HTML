#!/bin/sh
set -eu
make tests/markforge_tests
if command -v valgrind >/dev/null 2>&1; then
  valgrind --leak-check=full --error-exitcode=7 ./tests/markforge_tests
else
  echo "valgrind not installed; tests built and ready"
fi
