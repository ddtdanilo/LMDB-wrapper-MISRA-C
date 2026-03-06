# Project Context

## Origin

This project was created to integrate the LMDB (Lightning Memory-Mapped Database) library into a software ecosystem that requires strict compliance with the MISRA C code safety standard.

LMDB is a highly efficient C library for managing key-value databases backed by persistent memory (memory-mapped). However, LMDB does not fully comply with the MISRA C standard, which is a mandatory requirement for the higher-level layers of the application.

## Purpose

Develop a **wrapper** in C over LMDB that:

1. Exposes the same LMDB functionalities needed by the project.
2. Complies with **MISRA C** standards regarding parameter validation, error handling, and return values.
3. Serves as a base component for a higher-level system that manages **creation, reading, and validation of blocks** with a blockchain-like architecture.

## Use Cases

### Primary

- **Persistent storage**: LMDB acts as the persistent memory storage engine for records and blocks.
- **Blockchain simulation**: The higher-level system consumes this wrapper to manage block chains, branch validation, and CRUD operations on blocks.
- **Server component**: This wrapper is a base module that is eventually integrated as a component within a server and consumed by higher-level systems.

### Real-World LMDB Applications

LMDB is used in production by major projects:

- **Monero** — stores its entire blockchain (blocks, transactions, outputs) in LMDB.
- **OpenLDAP** — LMDB was originally created as the backend storage engine for OpenLDAP.
- **Postfix** — uses LMDB for mail alias maps and configuration lookup tables.
- **Caffe / Caffe2** — stores image datasets in LMDB for high-throughput deep learning training.

### Target Environments for This Wrapper

- **Automotive** (AUTOSAR, ISO 26262) — functional safety requirements demand MISRA C compliance.
- **Medical devices** (IEC 62304) — regulatory compliance requires validated, traceable code.
- **Industrial control** (IEC 61508) — safety instrumented systems require defensive programming.
- **Aerospace** (DO-178C) — airborne software requires strict coding standards.
- **IoT / firmware** — resource-constrained devices benefit from LMDB's ~30KB footprint and this wrapper's safety guarantees.

## MISRA C Standard

MISRA C is a code safety standard for C widely used in the automotive, aerospace, and embedded systems industries. The key rules that apply to this project:

- **Mandatory validation of all input parameters** in every function.
- **Specific error returns for null parameters** (null pointer).
- **Specific error returns for invalid parameters**.
- **Mandatory validation of return values** in all function calls.
- **No use of unsafe functions** (malloc without validation, etc.).
- **Explicit control flow** (no fall-through in switch, no goto).

## Technology Stack

| Component      | Technology             |
|----------------|------------------------|
| Language       | C (C99)                |
| Base library   | LMDB                   |
| Compiler       | GCC / Clang            |
| Build system   | CMake (>= 3.14)        |
| Testing        | CMocka (unit and integration tests) |
| CI/CD          | GitHub Actions          |
