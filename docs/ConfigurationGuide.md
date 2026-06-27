# Configuration Guide

## Status

The implemented configuration format is flat key/value text. Lines may use either `key=value` or `key: value`.

## Implemented

- Loading flat key/value files.
- In-memory configuration map.
- Basic config cache.
- Default values for a few renderer settings.
- Profile string override through environment lookup.
- Minimal validation that configuration version metadata exists.

## Not Implemented

JSON, YAML, full INI sections, imports, inheritance, schema validation, conflict detection and hot reload are not implemented in this alpha.
