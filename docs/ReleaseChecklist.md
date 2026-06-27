# Release Checklist

- [ ] Update version metadata.
- [ ] Run `make clean static-analysis test benchmark website-check`.
- [ ] Run sanitizer, Valgrind, fuzz and stress scripts where available.
- [ ] Build website with `make website-build`.
- [ ] Generate packages with `sh scripts/package-release.sh`.
- [ ] Verify checksums and release notes.
- [ ] Tag release using Semantic Versioning.
- [ ] Publish artifacts and documentation.
