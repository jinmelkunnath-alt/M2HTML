# Coding Standards

Public symbols use the `mf_` prefix. Types use `_t`. Headers use `MARKFORGE_<NAME>_H`. Public declarations are in headers; definitions are in matching source files. Include order is own header, MarkForge headers, then system headers. Document memory ownership. Use `mf_result_t` for recoverable errors. Prefer small composable functions, stable structs, explicit lengths, and no hidden global state except documented test helpers.
