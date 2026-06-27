# Development Workflow

Each phase extends the repository without redesigning existing modules. All changes must compile, pass tests, update documentation, and update `PROJECT_MANIFEST.md`. Parser work in Prompt 2 should add tokenizer, lexer, token stream, parser, AST, and syntax modules that reuse this foundation.
