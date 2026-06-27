# PROJECT_MANIFEST.md

Authoritative repository index for **MarkForge 0.5.0-alpha**. This manifest reflects implemented functionality only.

## Release Identity

- Product: MarkForge
- Version: 0.5.0-alpha
- Status: public alpha
- License: MIT
- Core language: ANSI C
- Website: React, TypeScript, Vite, Tailwind, local WASM version-check module
- Runtime model: offline-first; no API keys, telemetry or remote backend required at runtime
- Supported native platforms targeted by code: Linux, macOS, Windows and WSL, with some alpha limitations noted in docs

## Top-Level Folders

`include/markforge`, `src`, `tests`, `benchmarks`, `examples`, `plugins`, `configs`, `themes`, `assets`, `assets/diagrams`, `docs`, `docs/release`, `tools`, `scripts`, `website`, `release`, `.github`, `build`.

## Public Headers

Foundation: `common.h`, `memory.h`, `containers.h`, `strings.h`, `fs.h`, `logging.h`, `diagnostics.h`, `cli.h`, `config.h`, `platform.h`, `util.h`, `test.h`.

Parser and AST: `tokenizer.h`, `token_stream.h`, `ast.h`, `parser.h`, `renderer.h`.

Rendering and alpha extensibility: `renderers.h`, `renderer_manager.h`, `config_engine.h`, `theme.h`, `plugin.h`, `hooks.h`, `commands.h`, `devtools.h`.

Umbrella header: `markforge.h`.

## Source Files

Foundation: `src/common.c`, `src/memory.c`, `src/containers.c`, `src/strings.c`, `src/fs.c`, `src/logging.c`, `src/diagnostics.c`, `src/cli.c`, `src/config.c`, `src/platform.c`, `src/util.c`, `src/test.c`.

Parser/AST: `src/tokenizer.c`, `src/token_stream.c`, `src/ast.c`, `src/parser.c`.

Render/config/plugin/theme/hooks: `src/renderer.c`, `src/renderers.c`, `src/renderer_manager.c`, `src/config_engine.c`, `src/theme.c`, `src/plugin.c`, `src/hooks.c`, `src/commands.c`.

Developer tooling and CLI: `src/devtools.c`, `src/cli_app.c`, `tools/markforge.c`.

## Implemented Parser Scope

- Tokenizer with positions and UTF-8 validation.
- Token stream API.
- Parser token-stream validation for unclosed inline backticks.
- Line-oriented block parser for headings, paragraphs, simple lists, blockquotes, code fences, horizontal rules, table text nodes and HTML block text nodes.
- Basic inline parsing for bold, italic, strike, highlight, inline code, links, images and inline math.
- AST node ownership for text and link/image metadata is arena-backed after stabilization.

Not implemented in this alpha: production lexer module, full CommonMark compliance, full parser recovery, nested list semantics, full table rows/cells, footnotes, task lists, complete HTML block rules, automatic links/emails and dynamic extension syntax.

## Renderers

Implemented built-ins: HTML, JSON, XML, plain text and simple Markdown formatter.

Stabilization notes: HTML text/attribute escaping is implemented, unsafe URL schemes in links/images are replaced with safe output, JSON string escaping handles common control characters, and output is deterministic for the supported AST subset.

## CLI Commands

Implemented command names: `parse`, `validate`, `dump-tokens`, `dump-ast`, `render-html`, `render-json`, `render-xml`, `render-text`, `format`, `list-renderers`, `plugins`, `themes`, `config`, `validate-config`, `render`, `lint`, `benchmark`, `profile`, `stats`, `watch`, `serve`, `shell`, `theme`, `list-themes`, `renderer`, `plugin`, `diagnostics`, `dump-tree`, `dump-json`, `license`, `about`, `examples`, `doctor`, `init`.

Alpha limitations: command-specific option parsing is basic; shell, watch and preview server are smoke/development utilities, not production daemons.

## Configuration, Themes and Plugins

- Configuration format: flat `key=value` or `key: value` text.
- Theme support: key/value theme records and registration lookup.
- Plugin support: static/local metadata registration with init/shutdown callbacks. Dynamic plugin loading and sandboxing are not implemented.
- Hooks: callback registration and event emission.

## Website

Website configuration and build files: `website/package.json`, `website/package-lock.json`, `website/tsconfig.json`, `website/vite.config.ts`, `website/tailwind.config.ts`, `website/postcss.config.cjs`, `website/index.html`, `website/404.html`.

Pages: `website/src/pages/Home.tsx`, `Playground.tsx`, `Documentation.tsx`, `Examples.tsx`, `Benchmarks.tsx`, `Architecture.tsx`, `SimplePage.tsx`.

Components: `Layout.tsx`, `Router.tsx`, `ui.tsx`, `CommandPalette.tsx`, `MarkdownPlayground.tsx`, `PerformanceDashboard.tsx`.

Hooks/utilities/data: `useMarkForge.ts`, `useCommandPalette.ts`, `useLocalFile.ts`, `storage.ts`, `examples.ts`, `docs.ts`, `vite-env.d.ts`.

WASM: `website/public/wasm/markforge.wasm` exports a version function only. `website/src/wasm/markforgeWasm.ts` uses a TypeScript parser approximation for the playground. Full C parser WASM integration is planned, not implemented.

## Tests

Native test modules: `test_memory.c`, `test_containers.c`, `test_strings.c`, `test_fs.c`, `test_logging.c`, `test_diagnostics.c`, `test_cli.c`, `test_config.c`, `test_platform.c`, `test_util.c`, `test_rendering.c`, `test_extensibility.c`, `test_devtools.c`, `test_release_quality.c`, `test_runner.c`.

Website tests: `website/src/__tests__/wasm.test.ts`, `website/src/__tests__/docs.test.ts`.

## Benchmarks

`benchmarks/benchmark_runner.c` provides smoke benchmarks for vector insertion, parse loop and HTML render loop. These are baseline development benchmarks, not comprehensive performance claims.

## Documentation

Core: `README.md`, `CONTRIBUTING.md`, `CHANGELOG.md`, `CODE_OF_CONDUCT.md`, `SECURITY.md`.

Architecture and development: `docs/Architecture.md`, `FolderStructure.md`, `MemoryModel.md`, `CodingStandards.md`, `BuildGuide.md`, `DevelopmentWorkflow.md`, `PerformanceGuide.md`, `StaticAnalysis.md`, `APIReference.md`, `MigrationGuide.md`, `FAQ.md`, `Troubleshooting.md`, `VersioningPolicy.md`, `SupportPolicy.md`, `ReleaseChecklist.md`.

Parser/rendering/tooling/website documents remain in `docs/` and should describe alpha status where applicable. Release notes: `docs/release/MarkForge-0.5.0-alpha.md`.

## Scripts

Build/test/release: `check-format.sh`, `run-tests.sh`, `version.sh`, `package.sh`, `package-release.sh`, `checksum.sh`, `release-notes.sh`, `release.sh`.

Quality/security/performance: `analyze.sh`, `sanitize.sh`, `valgrind.sh`, `fuzz.sh`, `stress.sh`.

## GitHub Workflows and Templates

Workflows: `.github/workflows/ci.yml`, `.github/workflows/pages.yml`, `.github/workflows/release.yml`.

Templates/community files: issue templates, pull request template, discussion template and funding file.

## Release Artifacts

Generated alpha package copies are stored in `release/`:

- `release/markforge-0.5.0-alpha-source.tar`
- `release/markforge-0.5.0-alpha-source.zip`
- `release/CHECKSUMS.txt`
- `release/RELEASE_NOTES_0.5.0-alpha.md`

## Release Gate Status

- Native build and tests pass under the Makefile.
- Documentation has been corrected to identify alpha limitations.
- Critical AST metadata lifetime bug has been fixed.
- Basic unsafe URL handling has been added to HTML rendering.
- MarkForge is approved for public alpha publication, not stable production release.
