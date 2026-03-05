# Project Architecture

## Directory Structure

```
LMDB-wrapper-MISRA-C/
├── CLAUDE.md              # AI/development rules
├── CMakeLists.txt         # Main build system
├── README.md              # Main documentation
├── LICENSE                # MIT License
├── CONTRIBUTING.md        # Contribution guidelines
├── docs/                  # Detailed documentation
│   ├── context.md         # Context and background
│   └── architecture.md    # This file
├── todo/                  # Task tracking
│   └── plan.md            # Detailed task list
├── include/               # Public headers
│   └── lmdb_wrapper.h     # Wrapper public API
├── src/                   # Source code
│   └── lmdb_wrapper.c     # Wrapper implementation
├── tests/                 # Tests
│   └── test_lmdb_wrapper.c # Unit and integration tests
├── examples/              # Example programs
│   └── basic_usage.c      # Basic usage example
├── external/              # External dependencies
│   └── lmdb/              # LMDB source (downloaded by CMake)
├── cmake/                 # CMake helper modules
│   └── FetchLMDB.cmake    # Script to download LMDB
└── .github/               # GitHub configuration
    ├── workflows/         # CI/CD workflows
    └── ISSUE_TEMPLATE/    # Issue templates
```

## Architecture Layers

```
┌─────────────────────────────────┐
│   Higher-Level System           │
│   (Blockchain, validation)      │
├─────────────────────────────────┤
│   LMDB Wrapper (MISRA C)       │  ◄── This project
│   - Parameter validation        │
│   - Robust error handling       │
│   - Safe and consistent API     │
├─────────────────────────────────┤
│   LMDB (original library)      │
│   - Key-value engine            │
│   - Memory-mapped I/O           │
└─────────────────────────────────┘
```

## Data Flow

1. The higher-level system calls wrapper functions (e.g., `lmdbWrapperPut()`).
2. The wrapper validates all input parameters according to MISRA C.
3. If validation passes, the wrapper invokes the corresponding LMDB function.
4. The wrapper validates the LMDB return value.
5. The wrapper returns a standardized error code to the caller.

## Error Codes

The wrapper defines its own error codes that encapsulate LMDB errors and add MISRA C categories:

- `LMDB_WRAPPER_SUCCESS` (0): Operation successful.
- `LMDB_WRAPPER_ERR_NULL_PARAM`: Null parameter detected.
- `LMDB_WRAPPER_ERR_INVALID_PARAM`: Parameter with invalid value.
- `LMDB_WRAPPER_ERR_ENV_CREATE`: Failed to create LMDB environment.
- `LMDB_WRAPPER_ERR_ENV_OPEN`: Failed to open LMDB environment.
- `LMDB_WRAPPER_ERR_TXN_BEGIN`: Failed to begin transaction.
- `LMDB_WRAPPER_ERR_DBI_OPEN`: Failed to open database handle.
- `LMDB_WRAPPER_ERR_PUT`: Failed to write record.
- `LMDB_WRAPPER_ERR_GET`: Failed to read record.
- `LMDB_WRAPPER_ERR_DEL`: Failed to delete record.
- `LMDB_WRAPPER_ERR_TXN_COMMIT`: Failed to commit transaction.
- `LMDB_WRAPPER_ERR_NOT_FOUND`: Record not found.
- `LMDB_WRAPPER_ERR_MAP_FULL`: Database map is full.

## Supported Operations

| Operation              | Function                    |
|------------------------|-----------------------------|
| Create/open environment| `lmdbWrapperEnvCreate()`    |
| Close environment      | `lmdbWrapperEnvClose()`     |
| Store record           | `lmdbWrapperPut()`          |
| Retrieve record        | `lmdbWrapperGet()`          |
| Delete record          | `lmdbWrapperDel()`          |
| Error to string        | `lmdbWrapperStrerror()`     |
