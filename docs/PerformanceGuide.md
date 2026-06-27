# Performance Guide

MarkForge currently provides development smoke benchmarks, not comprehensive production performance certification.

## Implemented Measurements

- `make benchmark` runs vector insertion, repeated parse and repeated HTML render loops.
- `tools/markforge profile file.md` reports parse time, render time, approximate memory use, node count, line count and word count.

## Current Limitations

- File reading is whole-file based.
- Token streams are fully materialized.
- AST allocation uses a fixed-size arena by default.
- Large-file support is not production-complete.

## Recommended Use During Alpha

Use the benchmark numbers for regression comparison within the same machine/environment only. Do not treat them as cross-platform performance guarantees.
