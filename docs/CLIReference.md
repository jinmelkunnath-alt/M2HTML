# MarkForge Developer Tooling

MarkForge Prompt 4 adds professional offline developer tooling around the parser, AST, renderer, configuration, theme, plugin, and diagnostics systems. The implementation is written in ANSI C and remains self-contained.

## Capabilities

- Professional CLI command suite with global flags: --help, --verbose, --quiet, --json, --color, --no-color, and --config.
- Terminal panels, tables, progress bars, AST tree views, and adaptive color control.
- Interactive shell for repeated local commands.
- Watch mode built on incremental parsing and rendering cache APIs.
- Localhost-only preview server implemented in C.
- Profiler measuring parse time, render time, memory, nodes, lines, words, and nesting.
- Benchmark reports with CSV, JSON, and Markdown export.
- Lint and validation diagnostics integrated with the diagnostics framework.
- Inspectors for tokens, AST, performance, renderer, plugin, config, memory, and diagnostics workflows.
- Local-only plugin management and release automation scripts.

## Offline Operation

No API keys, telemetry, SaaS services, cloud databases, authentication providers, or external servers are required.

## Commands

parse, render, format, lint, validate, benchmark, profile, stats, watch, serve, shell, theme, renderer, plugin, config, diagnostics, dump-tokens, dump-ast, dump-tree, dump-json, help, version, license, about, examples, doctor, init.
