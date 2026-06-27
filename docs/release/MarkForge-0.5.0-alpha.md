# MarkForge 0.5.0-alpha Release Notes

MarkForge 0.5.0-alpha is a public alpha for the offline-first Markdown processing toolkit. It is not a stable 1.0 release.

## Implemented

- ANSI C support libraries and diagnostics.
- Tokenizer, token stream and line-oriented parser for a limited Markdown subset.
- AST with parent/child/sibling links, visitors and statistics.
- Basic HTML, JSON, XML, text and Markdown-format renderers.
- CLI executable, linter, validator, profiler and benchmark smoke tooling.
- Static website/playground prototype.
- Release scripts and CI workflow scaffolding.

## Known Limitations

- Not CommonMark-complete.
- No production lexer module.
- Plugin system is static/local metadata only.
- Configuration is flat key/value only.
- Website playground does not yet execute the full C parser in WebAssembly.
- Large-file support is limited by whole-file reads and in-memory token/AST storage.

## Release Decision

Approved as alpha for public testing and contribution, not stable production use.
