# Lexer

## Status

A separate production lexer module is not implemented in this alpha. MarkForge currently provides a tokenizer and token stream. The line-oriented parser performs its own block recognition.

## Implemented Related APIs

- `mf_tokenizer_t`
- `mf_token_stream_t`

## Planned

A future lexer may classify tokenizer output into semantic Markdown tokens while preserving the existing public parser API.
