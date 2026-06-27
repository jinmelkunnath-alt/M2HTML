#!/bin/sh
set -eu
make tools/markforge
work=${TMPDIR:-/tmp}/markforge-fuzz.md
: > "$work"
i=0
while [ $i -lt ${MARKFORGE_FUZZ_CASES:-50} ]; do
  printf '# fuzz %s\n\n* item %s\n`code` **bold** [link](x)\n' "$i" "$i" > "$work"
  ./tools/markforge validate "$work" >/dev/null || true
  ./tools/markforge render-html "$work" >/dev/null
  i=$((i+1))
done
rm -f "$work"
echo "fuzz smoke complete"
