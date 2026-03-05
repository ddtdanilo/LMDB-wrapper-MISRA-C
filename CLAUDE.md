# CLAUDE.md - Project Rules and Guidelines

## Project Overview

LMDB wrapper library in C compliant with MISRA C safety standards. Used as a base component for blockchain-like block management systems.

## Language and Tooling

- **Language**: C (C99 standard)
- **Compiler**: GCC / Clang with strict warnings
- **Build system**: CMake (minimum 3.14)
- **Base library**: LMDB (Lightning Memory-Mapped Database)
- **Always use the folder todo/*.md for task tracking and planning**
- **All code, comments, documentation, and commit messages must be in English**

## MISRA C Compliance Rules (MANDATORY)

Every function in this wrapper MUST follow these rules:

1. **Validate ALL input parameters** at the start of every function.
2. **NULL pointer check**: Return `LMDB_WRAPPER_ERR_NULL_PARAM` for any null pointer argument.
3. **Invalid parameter check**: Return `LMDB_WRAPPER_ERR_INVALID_PARAM` for any out-of-range or semantically invalid argument.
4. **Validate ALL return values**: Every call to an LMDB function or standard library function must have its return value checked.
5. **No implicit fall-through** in `switch` statements.
6. **No `goto` statements**.
7. **No dynamic memory allocation without validation** (every `malloc`/`calloc` result must be checked).
8. **All variables must be initialized** at declaration.
9. **Use explicit types** (`uint32_t`, `int32_t`, `size_t`) instead of bare `int`/`long`.
10. **Single return point per function** (MANDATORY, no exceptions). Use a `result` variable and `if/else` chains.

## Coding Standards

- Use `camelCase` for all function names, variables, and type names.
- Use `UPPER_SNAKE_CASE` for macros, constants, and enum values.
- Prefix all public API functions with `lmdbWrapper`.
- Prefix all error codes with `LMDB_WRAPPER_ERR_` (except `LMDB_WRAPPER_SUCCESS`).
- Type names use camelCase with `_t` suffix (e.g., `lmdbWrapperErr_t`).
- **Indentation: 3 spaces** (no tabs).
- **Closing brace comments**: Every closing `}` of a function, `if`, `else`, `for`, `while`, or `switch` must have a comment describing what it closes.
- Header files must use include guards with `#ifndef` / `#define` / `#endif`.
- Every public function must have a documentation comment in the header file.
- Every source file must include a file-level comment with a brief description.

## Error Handling Pattern

```c
lmdbWrapperErr_t lmdbWrapperExample(const param_t *param)
{
   lmdbWrapperErr_t result = LMDB_WRAPPER_SUCCESS;

   /* Validate input parameters (MISRA C) */
   if (param == NULL) {
      result = LMDB_WRAPPER_ERR_NULL_PARAM;
   } else if (param->value < 0) {
      result = LMDB_WRAPPER_ERR_INVALID_PARAM;
   } /* if parameter validation */

   /* Call LMDB and validate return value (MISRA C) */
   if (result == LMDB_WRAPPER_SUCCESS) {
      int rc = mdb_some_function(...);
      if (rc != 0) {
         result = LMDB_WRAPPER_ERR_LMDB;
      } /* if rc */
   } /* if result success */

   return result;
} /* lmdbWrapperExample */
```

## Testing Requirements

- Every public API function must have at least one test.
- Tests must cover: success path, null parameters, invalid parameters, and LMDB error conditions.
- Tests follow the same coding standards (camelCase, 3-space indent, closing brace comments).
- Minimum test scenarios:
  - Create a database environment.
  - Open an existing database.
  - Create (put) records.
  - Read (get) records.
  - Delete records.
  - Update (overwrite) records.
  - Verify error handling for null/invalid inputs on ALL functions.

## Build Commands

```bash
# Configure
cmake -B build -DCMAKE_BUILD_TYPE=Debug

# Build
cmake --build build

# Run tests
cd build && ctest --output-on-failure
```

## Git Rules

- **NEVER include AI as co-author in commits.** No `Co-Authored-By` lines referencing any AI tool.
- Write clear, concise commit messages describing the change.
- Use conventional commit style: `feat:`, `fix:`, `test:`, `docs:`, `chore:`.

## File Organization

- `include/` — Public header files (.h)
- `src/` — Implementation files (.c)
- `tests/` — Test files (.c)
- `examples/` — Example programs (.c)
- `docs/` — Project documentation (.md)
- `todo/` — Task tracking (.md)
- `external/` — Third-party dependencies (managed by CMake)
- `cmake/` — CMake helper modules

## Do NOT

- Do not modify LMDB source code directly.
- Do not use `void*` without explicit casting and documentation.
- Do not suppress compiler warnings.
- Do not commit build artifacts (build/, *.o, *.a).
- Do not use platform-specific APIs without abstraction.
- Do not use multiple return statements in a function.
