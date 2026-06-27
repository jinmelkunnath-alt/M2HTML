# Build Guide

Run `make`, `make test`, and `make benchmark`. The default compiler flags include `-Wall -Wextra -Wpedantic`. CMake usage: `cmake -S . -B build`, `cmake --build build`, and `ctest --test-dir build`. GCC, Clang, MSVC, Linux, macOS, Windows, and WSL are supported by the architecture and CI configuration.
