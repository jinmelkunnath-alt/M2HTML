#!/bin/sh
set -eu
make tools/markforge
file=${TMPDIR:-/tmp}/markforge-stress.md
: > "$file"
i=0
while [ $i -lt ${MARKFORGE_STRESS_LINES:-5000} ]; do
  printf '## Section %s\n\nParagraph with **bold** and `code`.\n\n' "$i" >> "$file"
  i=$((i+1))
done
./tools/markforge profile "$file"
rm -f "$file"
