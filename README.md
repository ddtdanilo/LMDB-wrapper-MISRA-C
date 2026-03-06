# LMDB Wrapper - MISRA C

[![CI](https://github.com/ddtdanilo/LMDB-wrapper-MISRA-C/actions/workflows/ci.yml/badge.svg)](https://github.com/ddtdanilo/LMDB-wrapper-MISRA-C/actions/workflows/ci.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Release](https://img.shields.io/github/v/release/ddtdanilo/LMDB-wrapper-MISRA-C)](https://github.com/ddtdanilo/LMDB-wrapper-MISRA-C/releases)
[![Buy Me A Coffee](https://img.shields.io/badge/Buy%20Me%20A%20Coffee-support-yellow?logo=buy-me-a-coffee)](https://buymeacoffee.com/ddtdanilo)

A MISRA C compliant wrapper for [LMDB](https://github.com/LMDB/lmdb) (Lightning Memory-Mapped Database).

## Purpose

This library provides a safe, validated API layer over LMDB that enforces MISRA C coding standards. It serves as a base component for systems that require reliable key-value storage with strict error handling — such as blockchain-like block management architectures.

## Features

- Full MISRA C parameter validation on every function call
- Specific error codes for null parameters, invalid parameters, and LMDB errors
- All return values validated (no unchecked calls)
- Read API copies values into caller-owned buffers
- Simple API: create environment, put/get/delete records, close
- Automatic LMDB download via CMake (no manual dependency setup)

## API

| Function | Description |
|---|---|
| `lmdbWrapperEnvCreate()` | Create and open a database environment |
| `lmdbWrapperEnvClose()` | Close and destroy an environment |
| `lmdbWrapperPut()` | Store a key-value record |
| `lmdbWrapperGet()` | Copy a value by key into a caller buffer |
| `lmdbWrapperDel()` | Delete a record by key |
| `lmdbWrapperStrerror()` | Convert error code to string |

## Quick Start

### Prerequisites

- C compiler (GCC or Clang)
- CMake >= 3.14
- Git (for LMDB download)

### Build

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

### Run Tests

```bash
cd build && ctest --output-on-failure
```

Or run the test executable directly for verbose output:

```bash
./build/test_lmdb_wrapper
```

### Example Usage

```c
#include "lmdb_wrapper.h"
#include <sys/stat.h>
#include <string.h>

int main(void) {
    lmdbWrapperEnv_t *env = NULL;
    lmdbWrapperConfig_t config = {
        .path = "./my_database",
        .mapSize = 0,  /* wrapper default 10MB */
        .maxDbs = 0    /* default 1 */
    };
    char readVal[64] = {0};
    size_t readSize = 0;

    (void)mkdir("./my_database", 0755);

    lmdbWrapperErr_t err = lmdbWrapperEnvCreate(&env, &config);
    if (err != LMDB_WRAPPER_SUCCESS) {
        return 1;
    }

    /* Write */
    const char *key = "block_001";
    const char *val = "data...";
    err = lmdbWrapperPut(env, key, strlen(key), val, strlen(val));

    /* Read */
    err = lmdbWrapperGet(env, key, strlen(key),
                         readVal, sizeof(readVal), &readSize);

    /* Delete */
    err = lmdbWrapperDel(env, key, strlen(key));

    lmdbWrapperEnvClose(env);
    return 0;
}
```

## Project Structure

```
├── CLAUDE.md          # AI/development rules
├── CMakeLists.txt     # Build configuration
├── README.md          # This file
├── LICENSE            # MIT License
├── CONTRIBUTING.md    # Contribution guidelines
├── cmake/             # CMake modules
│   └── FetchLMDB.cmake
├── docs/              # Documentation
│   ├── architecture.md
│   └── context.md
├── examples/          # Example programs
│   └── basic_usage.c
├── include/           # Public headers
│   └── lmdb_wrapper.h
├── src/               # Implementation
│   └── lmdb_wrapper.c
├── tests/             # Tests
│   └── test_lmdb_wrapper.c
└── todo/              # Task tracking
    └── plan.md
```

## Error Codes

| Code | Meaning |
|---|---|
| `LMDB_WRAPPER_SUCCESS` | Operation succeeded |
| `LMDB_WRAPPER_ERR_NULL_PARAM` | A required parameter was NULL |
| `LMDB_WRAPPER_ERR_INVALID_PARAM` | A parameter had an invalid value |
| `LMDB_WRAPPER_ERR_NOT_FOUND` | Record not found |
| `LMDB_WRAPPER_ERR_ENV_CREATE` | Failed to create LMDB environment |
| `LMDB_WRAPPER_ERR_ENV_OPEN` | Failed to open LMDB environment |
| `LMDB_WRAPPER_ERR_TXN_BEGIN` | Failed to begin transaction |
| `LMDB_WRAPPER_ERR_DBI_OPEN` | Failed to open database handle |
| `LMDB_WRAPPER_ERR_PUT` | Failed to write record |
| `LMDB_WRAPPER_ERR_GET` | Failed to read record |
| `LMDB_WRAPPER_ERR_DEL` | Failed to delete record |
| `LMDB_WRAPPER_ERR_TXN_COMMIT` | Failed to commit transaction |
| `LMDB_WRAPPER_ERR_MAP_FULL` | Database map is full |
| `LMDB_WRAPPER_ERR_BUFFER_TOO_SMALL` | Output buffer is too small |

## Contributing

Contributions are welcome! Please see [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines on how to build, test, and submit changes.

## License

This project is licensed under the [MIT License](LICENSE).
