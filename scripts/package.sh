#!/bin/sh
set -eu
version=$(sh scripts/version.sh)
name="markforge-$version"
rm -rf "build/$name"
mkdir -p "build/$name"
cp -R include src docs examples plugins configs themes assets tools README.md LICENSE CHANGELOG.md PROJECT_MANIFEST.md Makefile CMakeLists.txt "build/$name/"
( cd build && tar -cf "$name.tar" "$name" )
echo "build/$name.tar"
