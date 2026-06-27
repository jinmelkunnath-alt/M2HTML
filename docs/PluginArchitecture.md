# Plugin Architecture

## Implemented

MarkForge alpha supports static/local plugin metadata registration and optional init/shutdown callbacks. Hooks can be registered and emitted by host code.

## Not Implemented

Dynamic library loading, dependency resolution, plugin isolation, sandbox execution and remote installation are not implemented.
