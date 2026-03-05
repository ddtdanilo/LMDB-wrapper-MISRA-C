# Copilot Instructions — LMDB Wrapper MISRA C

This is a MISRA C compliant wrapper for LMDB written in C99. Follow these rules strictly when generating code for this project.

## Coding Standards

- Use `camelCase` for functions, variables, and type names
- Use `UPPER_SNAKE_CASE` for macros, constants, and enum values
- Prefix public API functions with `lmdbWrapper`
- Prefix error codes with `LMDB_WRAPPER_ERR_` (except `LMDB_WRAPPER_SUCCESS`)
- Type names use `_t` suffix (e.g., `lmdbWrapperErr_t`)
- Indentation: 3 spaces (no tabs)
- Every closing `}` must have a comment describing what it closes
- All code, comments, and documentation in English

## MISRA C Rules (Mandatory)

1. Validate ALL input parameters at function start
2. Return `LMDB_WRAPPER_ERR_NULL_PARAM` for null pointers
3. Return `LMDB_WRAPPER_ERR_INVALID_PARAM` for invalid values
4. Check ALL return values from LMDB and standard library functions
5. Single return point per function — use `result` variable with `if/else` chains
6. No `goto` statements
7. No implicit fall-through in `switch`
8. All variables initialized at declaration
9. Use explicit types: `uint32_t`, `int32_t`, `size_t` (not bare `int`/`long`)

## Error Handling Pattern

```c
lmdbWrapperErr_t lmdbWrapperExample(const param_t *param)
{
   lmdbWrapperErr_t result = LMDB_WRAPPER_SUCCESS;

   if (param == NULL) {
      result = LMDB_WRAPPER_ERR_NULL_PARAM;
   } /* if null check */

   if (result == LMDB_WRAPPER_SUCCESS) {
      int rc = mdb_some_function(...);
      if (rc != 0) {
         result = LMDB_WRAPPER_ERR_LMDB;
      } /* if rc */
   } /* if result success */

   return result;
} /* lmdbWrapperExample */
```

## API Functions

- `lmdbWrapperEnvCreate()` — Create/open environment
- `lmdbWrapperEnvClose()` — Close/destroy environment
- `lmdbWrapperPut()` — Store key-value record
- `lmdbWrapperGet()` — Retrieve value by key
- `lmdbWrapperDel()` — Delete record by key
- `lmdbWrapperStrerror()` — Error code to string
