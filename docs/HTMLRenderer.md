# HTML Renderer

## Implemented

The HTML renderer supports the AST subset currently produced by the parser: headings, paragraphs, simple inline emphasis, inline code, code blocks, simple lists, links, images, blockquotes, horizontal rules, math block text and safe HTML escaping.

Unsafe URL schemes in links/images are replaced with safe output.

## Limitations

Full table rendering, footnotes, task lists, callouts, advanced HTML block rules and complete CommonMark semantics are not implemented in this alpha.
