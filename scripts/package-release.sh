#!/bin/sh
set -eu
version=$(sh scripts/version.sh)
rm -rf "build/release"
mkdir -p "build/release/markforge-$version"
cp -R include src docs examples plugins configs themes assets website tools scripts README.md LICENSE CHANGELOG.md SECURITY.md CONTRIBUTING.md CODE_OF_CONDUCT.md PROJECT_MANIFEST.md Makefile CMakeLists.txt .github "build/release/markforge-$version/"
( cd build/release && tar -cf "markforge-$version-source.tar" "markforge-$version" )
if command -v zip >/dev/null 2>&1; then ( cd build/release && zip -qr "markforge-$version-source.zip" "markforge-$version" ); fi
sh scripts/release-notes.sh "$version" > "build/release/RELEASE_NOTES_$version.md"
: > "build/release/CHECKSUMS.txt"
if command -v cksum >/dev/null 2>&1; then
  find build/release -maxdepth 1 -type f ! -name CHECKSUMS.txt -print | sort | while read file; do cksum "$file" >> "build/release/CHECKSUMS.txt"; done
fi
echo "release package directory: build/release"
