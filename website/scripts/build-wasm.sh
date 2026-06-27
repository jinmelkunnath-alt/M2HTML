#!/bin/sh
set -eu
mkdir -p public/wasm
if command -v emcc >/dev/null 2>&1; then
  emcc wasm/markforge_wasm.c -O3 -s STANDALONE_WASM=1 -s EXPORTED_FUNCTIONS='["_markforge_version_major","_markforge_render_html"]' -o public/wasm/markforge.wasm
else
  echo "emcc not found; keeping checked-in portable wasm module"
fi
