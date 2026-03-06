# AGENTS.md

These project rules are derived from `CLAUDE.md` and adapted for Codex. They supplement higher-priority system and developer instructions.

## Project Scope

- LMDB wrapper library in C for MISRA-oriented, safety-focused usage.
- The codebase is intended to remain small, explicit, and conservative.

## Language And Tooling

- Language: C99.
- Compilers: GCC and Clang with strict warnings enabled.
- Build system: CMake 3.14+.
- Base library: LMDB.
- All code, comments, documentation, and commit messages must be in English.
- If persistent task tracking is needed in the repo, use `todo/*.md`.

## MISRA-Oriented Rules

Apply these rules to wrapper code and tests unless a higher-priority instruction overrides them:

1. Validate all input parameters at function entry.
2. Return `LMDB_WRAPPER_ERR_NULL_PARAM` for null pointer arguments.
3. Return `LMDB_WRAPPER_ERR_INVALID_PARAM` for semantically invalid values.
4. Check return values from LMDB and standard library calls.
5. Avoid implicit fall-through in `switch`.
6. Do not use `goto`.
7. Validate all dynamic allocation results before use.
8. Initialize variables at declaration.
9. Prefer explicit fixed-width or size-aware types such as `uint32_t`, `int32_t`, and `size_t`.
10. Use a single return point per function in project C code.

## Coding Standards

- Use `camelCase` for functions, variables, and typedef names.
- Use `UPPER_SNAKE_CASE` for macros, constants, and enum values.
- Prefix public API functions with `lmdbWrapper`.
- Prefix wrapper error codes with `LMDB_WRAPPER_ERR_`, except `LMDB_WRAPPER_SUCCESS`.
- Use `_t` suffix for typedef names.
- Use 3-space indentation and no tabs.
- Add closing-brace comments for non-trivial control blocks and functions, matching the existing style.
- Public headers must use include guards.
- Every public function must be documented in the header.
- Each source file should start with a short file-level comment.
- Do not use `void *` casually; when necessary, cast explicitly and document ownership or lifetime expectations.

## Testing

- Every public API function needs test coverage.
- Cover success paths, null parameter handling, invalid parameter handling, and relevant LMDB error paths.
- Keep test code aligned with production code style.
- Core scenarios to preserve:
  - Environment creation.
  - Reopening an existing database.
  - Put/get/delete flows.
  - Record overwrite behavior.
  - Validation failures on all public APIs.

## Build And Verification

- Configure: `cmake -B build -DCMAKE_BUILD_TYPE=Debug`
- Build: `cmake --build build`
- Test: `cd build && ctest --output-on-failure`
- Do not suppress compiler warnings to make code pass.

## Git And Change Hygiene

- Never add AI co-author trailers.
- Use clear commit messages. Conventional commit prefixes are preferred: `feat:`, `fix:`, `test:`, `docs:`, `chore:`.
- Do not commit build artifacts such as `build/`, `*.o`, or `*.a`.

## File Layout

- `include/`: public headers.
- `src/`: implementation files.
- `tests/`: test sources.
- `examples/`: sample programs.
- `docs/`: project documentation.
- `todo/`: task tracking and planning notes.
- `external/`: third-party sources managed by CMake.
- `cmake/`: CMake helper modules.

## Do Not

- Do not modify LMDB upstream source directly unless explicitly requested.
- Do not introduce platform-specific APIs without a clear abstraction boundary.
- Do not weaken warnings or style requirements just to get a build through.
- Do not use multiple return statements in project C functions unless the user explicitly asks to relax that rule.
