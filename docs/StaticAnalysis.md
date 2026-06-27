# Static Analysis and Sanitizers

Supported release-quality analysis commands:

- `make static-analysis`: compiler syntax and warning pass.
- `sh scripts/analyze.sh`: compiler analysis plus clang-tidy/cppcheck when installed.
- `sh scripts/sanitize.sh`: AddressSanitizer and UndefinedBehaviorSanitizer test run.
- `sh scripts/valgrind.sh`: Valgrind leak check when installed.
- `sh scripts/fuzz.sh`: deterministic malformed/randomized parser smoke tests.
- `sh scripts/stress.sh`: large document stress profiling.

These tools are optional at runtime. MarkForge itself remains self-contained.
