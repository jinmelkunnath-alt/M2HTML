# Renderer Architecture

Renderers consume `mf_ast_t` and write through `mf_render_output_t`.

## Implemented

- HTML renderer.
- JSON renderer.
- XML renderer.
- Plain text renderer.
- Simple Markdown formatter.
- Renderer manager registration and lookup.

## Limitations

Renderers only cover the AST subset produced by the alpha parser. Advanced Markdown nodes may exist in the enum but are not produced or fully rendered in this alpha.
