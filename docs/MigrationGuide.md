# Migration Guide

## Current Status

MarkForge is currently `0.5.0-alpha`. There is no stable 1.0 API contract yet.

## From Earlier Alpha Snapshots

- Version metadata now reports `0.5.0-alpha`.
- Documentation has been corrected to describe implemented features rather than planned features.
- Link and image metadata in the AST is now copied into the AST arena.
- HTML rendering now replaces unsafe URL schemes with safe output.

## Compatibility Policy During Alpha

Public APIs are intended to remain source-compatible where practical, but changes may still occur before a stable 1.0 release if required for correctness or memory safety.
