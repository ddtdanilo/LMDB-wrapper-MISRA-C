# Task Plan - LMDB Wrapper MISRA C

## Phase 1: Documentation and Structure

- [x] Create directory structure (docs/, todo/, src/, include/, tests/, external/, cmake/)
- [x] Create docs/context.md with project context and background
- [x] Create docs/architecture.md with architecture and design
- [x] Create CLAUDE.md with project rules
- [x] Create todo/plan.md (this file)

## Phase 2: Build System

- [x] Create cmake/FetchLMDB.cmake to download LMDB automatically
- [x] Create main CMakeLists.txt with library and test targets
- [x] Create .gitignore to exclude build artifacts

## Phase 3: Wrapper Source Code

- [x] Create include/lmdb_wrapper.h with:
  - [x] Error type definition (enum lmdbWrapperErr_t)
  - [x] Opaque structure for wrapper context
  - [x] Public function declarations with documentation
- [x] Create src/lmdb_wrapper.c with:
  - [x] lmdbWrapperEnvCreate() - Create LMDB environment
  - [x] lmdbWrapperEnvClose() - Close environment
  - [x] lmdbWrapperPut() - Store key-value record
  - [x] lmdbWrapperGet() - Retrieve record by key
  - [x] lmdbWrapperDel() - Delete record by key
  - [x] lmdbWrapperStrerror() - Convert error code to string
  - [x] MISRA C validation in ALL functions

## Phase 4: Testing

- [x] Create tests/test_lmdb_wrapper.c with:
  - [x] Test: Create new database
  - [x] Test: Open existing database
  - [x] Test: Create records (put)
  - [x] Test: Read records (get)
  - [x] Test: Delete records (del)
  - [x] Test: Null parameter validation
  - [x] Test: Invalid parameter validation
  - [x] Test: Record not found handling

## Phase 5: Build and Local Testing

- [x] Build the project with CMake
- [x] Run all tests
- [x] No compilation errors
- [x] All 26 tests passed successfully

## Phase 6: Open Source Release

- [x] Translate all documentation to English
- [x] Add MIT License
- [x] Add CONTRIBUTING.md
- [x] Add GitHub issue and PR templates
- [x] Add CI/CD with GitHub Actions
- [x] Update README.md with badges and correct API names
- [x] Set GitHub repository metadata

## Phase 7: vcpkg Distribution

### Background

[vcpkg](https://github.com/microsoft/vcpkg) is Microsoft's C/C++ package manager, used by thousands of projects worldwide. Getting a port accepted into vcpkg makes the library installable with a single command (`vcpkg install <name>`) and integrates seamlessly with CMake via `find_package`.

### Repositories involved

| Repository | Purpose |
|---|---|
| [ddtdanilo/LMDB-wrapper-MISRA-C](https://github.com/ddtdanilo/LMDB-wrapper-MISRA-C) | This project — the MISRA C wrapper source code |
| [microsoft/vcpkg](https://github.com/microsoft/vcpkg) | The official vcpkg registry where the port was merged |
| [ddtdanilo/vcpkg](https://github.com/ddtdanilo/vcpkg) | Personal fork of vcpkg used to submit the PR |

### Timeline

1. **v1.0.0** — Initial port submitted as `lmdb-wrapper`
2. **Reviewer feedback** — [JavierMatosD](https://github.com/JavierMatosD) (Microsoft) requested renaming the port to `ddtdanilo-lmdb-wrapper` to avoid naming conflicts with the upstream LMDB project
3. **v1.0.1** — Redesigned `lmdbWrapperGet` API (caller-owned buffers instead of internal pointers), added `LMDB_WRAPPER_ERR_BUFFER_TOO_SMALL`, added cppcheck to CI
4. **Port approved** — [vicroms](https://github.com/vicroms) (Microsoft, vcpkg team member) approved the PR
5. **Merged** — PR merged into microsoft/vcpkg master

### PR and links

- **PR**: [microsoft/vcpkg#50323](https://github.com/microsoft/vcpkg/pull/50323) — "[ddtdanilo-lmdb-wrapper] Add new port"
- **Port files in vcpkg**: [ports/ddtdanilo-lmdb-wrapper](https://github.com/microsoft/vcpkg/tree/master/ports/ddtdanilo-lmdb-wrapper)
- **Version entry**: [versions/d-/ddtdanilo-lmdb-wrapper.json](https://github.com/microsoft/vcpkg/blob/master/versions/d-/ddtdanilo-lmdb-wrapper.json)

### Tasks

- [x] Fork microsoft/vcpkg to [ddtdanilo/vcpkg](https://github.com/ddtdanilo/vcpkg)
- [x] Create port files (portfile.cmake, vcpkg.json)
- [x] Submit PR [microsoft/vcpkg#50323](https://github.com/microsoft/vcpkg/pull/50323)
- [x] Address reviewer feedback (rename port to `ddtdanilo-lmdb-wrapper`)
- [x] Update port to v1.0.1 after API redesign
- [x] PR approved by vcpkg team member [vicroms](https://github.com/vicroms)
- [x] PR merged into microsoft/vcpkg
- [x] Sync this repo's documentation with vcpkg availability

## Phase 8: Next Release

- [ ] Rename GitHub repo (coordinate with vcpkg port update)
  - Current name: `LMDB-wrapper-MISRA-C`
  - Cannot rename without breaking the vcpkg port — the SHA512 in the [portfile](https://github.com/microsoft/vcpkg/blob/master/ports/ddtdanilo-lmdb-wrapper/portfile.cmake) is computed from the release tarball, and the tarball's internal directory name includes the repo name. A rename changes the tarball content and invalidates the hash.
  - Must be done together with a new version release (e.g. v1.0.2 or v1.1.0)
  - Steps:
    1. Rename repo on GitHub (old URL will redirect, but tarball changes)
    2. Tag new release (`git tag v<new-version>`)
    3. Compute new SHA512 from the release tarball
    4. Submit PR to [microsoft/vcpkg](https://github.com/microsoft/vcpkg) updating `REPO`, `SHA512`, and version in the port files
