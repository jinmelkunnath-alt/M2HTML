# Troubleshooting

## Build warnings

Run `make clean && make static-analysis`. The release target requires zero warnings with the default C flags.

## Website build

Run `make website-check` to verify the website tree and local WASM artifact. Run `make website-build` in an environment with Node.js and npm.

## Diagnostics

Use `tools/markforge lint file.md`, `tools/markforge validate file.md` and `tools/markforge diagnostics file.md` to inspect input issues.
