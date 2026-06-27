# Plugin SDK

## Status

The plugin API is an alpha static-registration interface. It supports metadata structs and init/shutdown callbacks registered from code.

## Implemented

- Plugin metadata.
- Major-version compatibility check.
- Plugin manager storing registered plugin records.
- Static lifecycle callbacks.
- Hook manager integration.
- Manifest discovery that records a local manifest filename as metadata.

## Not Implemented

Dynamic library loading, executable plugin installation, sandboxing, dependency resolution and plugin isolation are not implemented.
