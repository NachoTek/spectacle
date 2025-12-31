# Development Guide

## Prerequisites

See `docs/development-instructions.md` for the full dependency list.

## Build

```sh
cmake -S . -B build
cmake --build build
```

## Run

Launch the built binary from `build/` via your platform's standard method.

## Tests

```sh
ctest --test-dir build
```