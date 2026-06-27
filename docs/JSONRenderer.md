# JSON Renderer

The JSON renderer serializes AST nodes with IDs, type, text, optional metadata, line, column and children. It is deterministic for the supported AST subset and includes JSON string escaping for common control characters.

Diagnostics serialization is not currently included in renderer output.
