# Parsing Pipeline

Current alpha pipeline:

1. File/string input.
2. Tokenizer produces low-level tokens and position metadata.
3. Token stream stores tokens for inspection and validation.
4. Parser performs token-stream validation for selected diagnostics and line-oriented block recognition.
5. AST builder creates arena-owned nodes.
6. Renderers consume the AST.

The long-term goal is to move block and inline grammar recognition fully onto token stream / lexer layers.
