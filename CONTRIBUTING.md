# Contributing to LMDB Wrapper - MISRA C

Thank you for your interest in contributing! This document provides guidelines for contributing to the project.

## Getting Started

### Prerequisites

- C compiler (GCC or Clang)
- CMake >= 3.14
- Git

### Build and Test

```bash
# Configure
cmake -B build -DCMAKE_BUILD_TYPE=Debug

# Build
cmake --build build

# Run tests
cd build && ctest --output-on-failure

# Run static analysis
cppcheck --quiet --std=c99 --check-level=exhaustive \
  --enable=warning,performance,portability \
  --error-exitcode=1 --inline-suppr --suppress=missingInclude \
  --suppress=missingIncludeSystem -I include src include tests examples
```

All tests and static analysis checks must pass before submitting a pull request.

## Code Style

This project follows strict coding standards. Please refer to [CLAUDE.md](CLAUDE.md) for the complete set of rules. Key points:

- **camelCase** for all function names, variables, and type names
- **UPPER_SNAKE_CASE** for macros, constants, and enum values
- **3-space indentation** (no tabs)
- **Closing brace comments** on every `}`
- **Single return point** per function
- All code, comments, and documentation must be in **English**

## MISRA C Compliance

Every function must comply with MISRA C standards:

1. Validate all input parameters at the start of every function
2. Check for NULL pointers and return `LMDB_WRAPPER_ERR_NULL_PARAM`
3. Check for invalid parameters and return `LMDB_WRAPPER_ERR_INVALID_PARAM`
4. Validate all return values from LMDB and standard library calls
5. No `goto` statements
6. No implicit fall-through in `switch` statements
7. All variables must be initialized at declaration
8. Use explicit types (`uint32_t`, `int32_t`, `size_t`)

## Testing Requirements

- Every public API function must have at least one test
- Tests must cover: success path, null parameters, invalid parameters, and error conditions
- Tests follow the same coding standards as production code
- Static analysis must pass with `cppcheck`

## Pull Request Process

1. Fork the repository and create a feature branch from `main`
2. Make your changes following the code style and MISRA C rules
3. Add or update tests for any new or modified functionality
4. Ensure all tests pass locally
5. Write a clear commit message using conventional commit style (`feat:`, `fix:`, `test:`, `docs:`, `chore:`)
6. Open a pull request against `main`

## Releasing a New Version

This library is distributed via [vcpkg](https://github.com/microsoft/vcpkg) as `ddtdanilo-lmdb-wrapper`. When releasing a new version:

1. Update the version in `CMakeLists.txt`
2. Tag the release (`git tag v<version>`) and push the tag
3. Update `vcpkg-port/vcpkg.json` with the new version
4. Submit a PR to [microsoft/vcpkg](https://github.com/microsoft/vcpkg) updating the port files and version database

## Reporting Issues

When reporting a bug, please include:

- A clear description of the problem
- Steps to reproduce the issue
- Expected vs actual behavior
- Your environment (OS, compiler, CMake version)

For feature requests, describe the use case and why the feature would be valuable.

## License

By contributing, you agree that your contributions will be licensed under the [MIT License](LICENSE).
