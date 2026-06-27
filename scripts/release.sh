#!/bin/sh
set -eu
make clean
make static-analysis
make test
artifact=$(sh scripts/package.sh)
sh scripts/checksum.sh "$artifact" > "$artifact.checksum"
sh scripts/release-notes.sh > "build/release-notes.md"
echo "release artifacts generated in build/"
