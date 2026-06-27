# MarkForge Subsystem

This document describes a MarkForge subsystem implemented in the self-contained C repository. The subsystem integrates with the stable Prompt 1 foundation and the parser, AST, renderer, configuration, theme, plugin, and CLI pipeline.

## Responsibilities

- Preserve the AST as the single source of truth.
- Use existing memory, diagnostics, logging, CLI, container, and string APIs.
- Avoid remote services, telemetry, API keys, or network dependencies.
- Maintain deterministic output suitable for tests, CLI use, and future web playground integration.

## Public API

Public declarations are indexed in PROJECT_MANIFEST.md and exposed through include/markforge/markforge.h.
