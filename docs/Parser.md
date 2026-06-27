# Parser

## Status

Implemented as an alpha, line-oriented parser that uses the tokenizer/token stream for tokenization, statistics and selected diagnostics. The current block parser still scans source lines for grammar recognition and is not a full CommonMark parser.

## Implemented

- Headings using `#` prefixes.
- Paragraphs.
- Simple unordered and ordered list items.
- Blockquotes.
- Horizontal rules.
- Code fences.
- Basic inline code, bold, italic, strike, highlight, links, images and inline math recognition.
- Diagnostics for invalid UTF-8 from the tokenizer and unclosed inline backticks from token-stream validation.

## Partial or Planned

- Full recursive-descent parsing over semantic lexer tokens.
- CommonMark compliance.
- Nested list semantics.
- Full tables, footnotes, task lists, callouts, spoilers, superscript/subscript, automatic links and full HTML block rules.
- Rich recovery with error nodes and synchronization sets.
