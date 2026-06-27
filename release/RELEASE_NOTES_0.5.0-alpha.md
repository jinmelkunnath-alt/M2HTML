# MarkForge 0.5.0-alpha

# Changelog

## 0.5.0-alpha - 2026-06-26

- Reclassified MarkForge as an alpha release after engineering audit.
- Corrected README, manifest and key documentation to match implemented functionality.
- Fixed AST lifetime for link/image metadata by copying metadata into the AST arena.
- Added token-stream validation diagnostic for unclosed inline backticks.
- Updated CLI validation to report parser/linter diagnostics.
- Hardened HTML rendering for unsafe URL schemes.
- Improved JSON string escaping.
- Fixed a system allocator alignment/free mismatch risk.
- Added pool allocation overflow and pointer-range checks.

## 0.4.0-alpha - 2026-06-26

- Added CLI executable, developer tooling APIs, lint/profiling/benchmark smoke tools, watch/preview scaffolding and release scripts.

## 0.3.0-alpha - 2026-06-26

- Added AST-driven renderers, renderer manager, configuration/theme/plugin scaffolding and hooks.

## 0.2.0-alpha - 2026-06-26

- Added tokenizer, token stream, line-oriented parser and AST.

## 0.1.0-alpha - 2026-06-26

- Created repository foundation, ANSI C support libraries, tests, documentation skeleton, Make and CMake build files.
