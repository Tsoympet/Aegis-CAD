# Repository audit (2024-04-27)

## Summary
- Cleaned up the primary application entry point so only the Qt application launcher is compiled.
- Rebuilt the Qt test harness to remove duplicate `main` definitions and restore missing helpers/macros.

## Findings and fixes
### Corrupted application entry point
- `src/app/main.cpp` contained two interleaved `main` functions, which would not compile and obscured the intended Qt startup path.
- The file now defines a single Qt-aware `main` that configures the surface format, initializes logging, and launches `MainWindow`.

### Broken test harness
- `tests/CoreTests.cpp` had two separate `main` implementations, an unterminated benchmark block, and no tolerance helper for geometric comparisons, leaving the test suite uncompilable.
- The file now uses one orchestrating `main`, explicit tolerance checking, and well-formed Qt test slots for both core and scripting coverage.

## Next steps
- Run the Qt/OpenCascade test suite in an environment with the required GUI/toolkit dependencies to verify the restored tests.
