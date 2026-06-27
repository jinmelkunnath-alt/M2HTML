# Security Policy

MarkForge is offline-first and does not require API keys, cloud services, remote databases, telemetry, analytics, authentication providers or runtime internet connectivity.

## Supported Versions

| Version | Supported |
| --- | --- |
| 0.5.0-alpha | Evaluation and security feedback accepted |
| 1.0.x | Not released |

## Reporting a Vulnerability

Please report security issues privately to the maintainers. Include version, platform, compiler, input document, command line, observed behavior and expected behavior.

## Current Alpha Security Notes

- The parser is not CommonMark-complete and should not yet be treated as hardened for hostile input at scale.
- HTML rendering escapes text and attributes and replaces unsafe link/image URL schemes with safe output.
- The preview server is localhost-only and POSIX-only in this alpha.
- Plugin support is static/local metadata registration only; dynamic plugin loading is not implemented.
- Configuration is flat key/value text only.

## Recommended Audit Commands

```sh
make static-analysis
sh scripts/analyze.sh
sh scripts/sanitize.sh
sh scripts/valgrind.sh
sh scripts/fuzz.sh
sh scripts/stress.sh
```
