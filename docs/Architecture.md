# Architecture

MarkForge is an alpha-stage offline Markdown toolkit with a portable ANSI C core.

## Implemented Layers

- Foundation libraries: memory, containers, strings, filesystem, diagnostics, logging and CLI helpers.
- Tokenizer and token stream.
- Line-oriented parser that produces an AST.
- AST with visitor traversal and statistics.
- Renderer manager and built-in renderers.
- Flat key/value configuration and static plugin metadata scaffolding.
- CLI and developer tooling.
- Static website prototype.

## Current Architectural Limitation

The parser initializes and validates a token stream, but block grammar recognition is still line-oriented. A future parser should move grammar recognition fully onto token stream / lexer layers without changing the public parser API.
