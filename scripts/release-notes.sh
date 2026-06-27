#!/bin/sh
set -eu
version=${1:-$(sh scripts/version.sh)}
printf '# MarkForge %s\n\n' "$version"
sed -n '1,120p' CHANGELOG.md
