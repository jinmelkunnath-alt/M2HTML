# WebAssembly Guide

## Status

The website includes a small local WASM module that verifies browser-side WASM loading by exporting `markforge_version_major`. The full ANSI C parser is not currently exposed to the playground through WASM.

## Implemented

- `website/public/wasm/markforge.wasm`: local checked-in WASM verification module.
- `website/src/wasm/markforgeWasm.ts`: loader plus TypeScript parser approximation used by the playground.
- `website/wasm/markforge_wasm.c`: C bridge source for future Emscripten builds.

## Planned

Expose the real C parser and renderers to the browser through stable WASM exports.
