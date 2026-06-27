# Tokenizer

The tokenizer is implemented in `src/tokenizer.c` and exposed through `include/markforge/tokenizer.h`.

## Implemented

- Byte offsets, line and column tracking.
- CRLF and LF newline handling.
- UTF-8 validation for non-ASCII sequences.
- Basic Markdown punctuation token types.
- Peek, checkpoint and restore.

## Limitations

- Restore retokenizes from the beginning.
- Tokens are low-level punctuation/text tokens, not full semantic Markdown tokens.
- Display column width for tabs/grapheme clusters is not implemented.
