/**
 * @file lmdb_wrapper.h
 * @brief MISRA C compliant wrapper for LMDB (Lightning Memory-Mapped Database).
 *
 * This wrapper provides a safe API over LMDB with strict parameter validation,
 * explicit error codes, and compliance with MISRA C coding standards.
 */

#ifndef LMDB_WRAPPER_H
#define LMDB_WRAPPER_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Error codes returned by all wrapper functions.
 *
 * MISRA C: All functions return explicit error codes.
 * Callers MUST check return values.
 */
typedef enum {
   LMDB_WRAPPER_SUCCESS           =   0, /**< Operation completed successfully */
   LMDB_WRAPPER_ERR_NULL_PARAM    =  -1, /**< A required parameter was NULL */
   LMDB_WRAPPER_ERR_INVALID_PARAM =  -2, /**< A parameter had an invalid value */
   LMDB_WRAPPER_ERR_ENV_CREATE    =  -3, /**< Failed to create LMDB environment */
   LMDB_WRAPPER_ERR_ENV_OPEN      =  -4, /**< Failed to open LMDB environment */
   LMDB_WRAPPER_ERR_TXN_BEGIN     =  -5, /**< Failed to begin transaction */
   LMDB_WRAPPER_ERR_DBI_OPEN      =  -6, /**< Failed to open database handle */
   LMDB_WRAPPER_ERR_PUT           =  -7, /**< Failed to write record */
   LMDB_WRAPPER_ERR_GET           =  -8, /**< Failed to read record */
   LMDB_WRAPPER_ERR_DEL           =  -9, /**< Failed to delete record */
   LMDB_WRAPPER_ERR_TXN_COMMIT    = -10, /**< Failed to commit transaction */
   LMDB_WRAPPER_ERR_NOT_FOUND     = -11, /**< Record not found */
   LMDB_WRAPPER_ERR_MAP_FULL      = -12, /**< Database map is full */
   LMDB_WRAPPER_ERR_INTERNAL      = -99  /**< Unexpected internal error */
} lmdbWrapperErr_t;

/**
 * @brief Opaque handle for an LMDB wrapper environment.
 *
 * Users must not access internal fields directly.
 */
typedef struct lmdbWrapperEnv lmdbWrapperEnv_t;

/**
 * @brief Configuration for creating a new LMDB environment.
 */
typedef struct {
   const char *path;      /**< Directory path for the database files (must exist) */
   size_t      mapSize;   /**< Maximum database size in bytes (0 = use LMDB default) */
   uint32_t    maxDbs;    /**< Maximum number of named databases (0 = use default of 1) */
} lmdbWrapperConfig_t;

/**
 * @brief Create and open an LMDB wrapper environment.
 *
 * @param[out] envOut   Pointer to receive the created environment handle.
 * @param[in]  config   Configuration parameters.
 * @return LMDB_WRAPPER_SUCCESS on success, or an error code.
 *
 * MISRA C: Validates envOut != NULL, config != NULL, config->path != NULL.
 */
lmdbWrapperErr_t lmdbWrapperEnvCreate(
   lmdbWrapperEnv_t **envOut,
   const lmdbWrapperConfig_t *config
);

/**
 * @brief Close and destroy an LMDB wrapper environment.
 *
 * @param[in] env  Environment handle to close. May be NULL (no-op).
 *
 * After this call, the env pointer is invalid and must not be used.
 */
void lmdbWrapperEnvClose(lmdbWrapperEnv_t *env);

/**
 * @brief Store a key-value record in the database.
 *
 * @param[in] env      Environment handle.
 * @param[in] key      Pointer to key data.
 * @param[in] keySize  Size of the key in bytes.
 * @param[in] val      Pointer to value data.
 * @param[in] valSize  Size of the value in bytes.
 * @return LMDB_WRAPPER_SUCCESS on success, or an error code.
 *
 * MISRA C: Validates all pointer parameters and sizes > 0.
 */
lmdbWrapperErr_t lmdbWrapperPut(
   lmdbWrapperEnv_t *env,
   const void *key,
   size_t keySize,
   const void *val,
   size_t valSize
);

/**
 * @brief Retrieve a value by key from the database.
 *
 * @param[in]  env         Environment handle.
 * @param[in]  key         Pointer to key data.
 * @param[in]  keySize     Size of the key in bytes.
 * @param[out] valOut      Pointer to receive the value data pointer.
 *                         The pointer is valid only until the next write operation.
 * @param[out] valSizeOut  Pointer to receive the value size.
 * @return LMDB_WRAPPER_SUCCESS on success,
 *         LMDB_WRAPPER_ERR_NOT_FOUND if key does not exist,
 *         or another error code.
 *
 * MISRA C: Validates all pointer parameters and keySize > 0.
 */
lmdbWrapperErr_t lmdbWrapperGet(
   lmdbWrapperEnv_t *env,
   const void *key,
   size_t keySize,
   void **valOut,
   size_t *valSizeOut
);

/**
 * @brief Delete a record by key from the database.
 *
 * @param[in] env      Environment handle.
 * @param[in] key      Pointer to key data.
 * @param[in] keySize  Size of the key in bytes.
 * @return LMDB_WRAPPER_SUCCESS on success,
 *         LMDB_WRAPPER_ERR_NOT_FOUND if key does not exist,
 *         or another error code.
 *
 * MISRA C: Validates all pointer parameters and keySize > 0.
 */
lmdbWrapperErr_t lmdbWrapperDel(
   lmdbWrapperEnv_t *env,
   const void *key,
   size_t keySize
);

/**
 * @brief Convert an error code to a human-readable string.
 *
 * @param[in] err  Error code to convert.
 * @return Pointer to a static string describing the error.
 *         Never returns NULL.
 */
const char *lmdbWrapperStrerror(lmdbWrapperErr_t err);

#ifdef __cplusplus
}
#endif

#endif /* LMDB_WRAPPER_H */
