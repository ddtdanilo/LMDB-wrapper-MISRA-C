/**
 * @file lmdb_wrapper.c
 * @brief MISRA C compliant wrapper implementation for LMDB.
 *
 * All functions validate input parameters and return values
 * according to MISRA C standards. Single return point enforced.
 */

#include "lmdb_wrapper.h"
#include "lmdb.h"

#include <stdlib.h>
#include <string.h>

/** Default map size: 10 MB */
#define DEFAULT_MAP_SIZE ((size_t)(10U * 1024U * 1024U))

/** Default max number of named databases */
#define DEFAULT_MAX_DBS ((uint32_t)1U)

/**
 * @brief Internal structure for the LMDB wrapper environment.
 */
struct lmdbWrapperEnv {
   MDB_env *mdbEnv;   /**< LMDB environment handle */
   MDB_dbi  mdbDbi;   /**< LMDB database handle */
   int      dbiOpen;  /**< Flag: 1 if dbi has been opened, 0 otherwise */
};

/**
 * @brief Open the default database within the environment.
 *
 * Internal helper. Assumes env is valid (caller has validated).
 *
 * @param[in,out] env  Wrapper environment with mdbEnv already open.
 * @return LMDB_WRAPPER_SUCCESS or an error code.
 */
static lmdbWrapperErr_t openDefaultDb(lmdbWrapperEnv_t *env)
{
   lmdbWrapperErr_t result = LMDB_WRAPPER_SUCCESS;
   MDB_txn *txn = NULL;
   int rc = 0;

   rc = mdb_txn_begin(env->mdbEnv, NULL, 0, &txn);
   if (rc != 0) {
      result = LMDB_WRAPPER_ERR_TXN_BEGIN;
   } /* if txn_begin failed */

   if (result == LMDB_WRAPPER_SUCCESS) {
      rc = mdb_dbi_open(txn, NULL, 0, &env->mdbDbi);
      if (rc != 0) {
         mdb_txn_abort(txn);
         result = LMDB_WRAPPER_ERR_DBI_OPEN;
      } /* if dbi_open failed */
   } /* if result success */

   if (result == LMDB_WRAPPER_SUCCESS) {
      rc = mdb_txn_commit(txn);
      if (rc != 0) {
         result = LMDB_WRAPPER_ERR_TXN_COMMIT;
      } else {
         env->dbiOpen = 1;
      } /* if txn_commit */
   } /* if result success */

   return result;
} /* openDefaultDb */

lmdbWrapperErr_t lmdbWrapperEnvCreate(
   lmdbWrapperEnv_t **envOut,
   const lmdbWrapperConfig_t *config)
{
   lmdbWrapperErr_t result = LMDB_WRAPPER_SUCCESS;
   lmdbWrapperEnv_t *env = NULL;
   int rc = 0;
   size_t mapSize = 0;
   uint32_t maxDbs = 0;

   /* MISRA C: Validate all input parameters */
   if (envOut == NULL) {
      result = LMDB_WRAPPER_ERR_NULL_PARAM;
   } else if (config == NULL) {
      result = LMDB_WRAPPER_ERR_NULL_PARAM;
   } else if (config->path == NULL) {
      result = LMDB_WRAPPER_ERR_NULL_PARAM;
   } else if (config->path[0] == '\0') {
      result = LMDB_WRAPPER_ERR_INVALID_PARAM;
   } /* if parameter validation */

   /* Initialize output to NULL */
   if (result == LMDB_WRAPPER_SUCCESS) {
      *envOut = NULL;
   } /* if result success */

   /* Allocate wrapper structure */
   if (result == LMDB_WRAPPER_SUCCESS) {
      env = (lmdbWrapperEnv_t *)calloc(1U, sizeof(lmdbWrapperEnv_t));
      if (env == NULL) {
         result = LMDB_WRAPPER_ERR_INTERNAL;
      } else {
         env->dbiOpen = 0;
      } /* if calloc */
   } /* if result success */

   /* Create LMDB environment */
   if (result == LMDB_WRAPPER_SUCCESS) {
      rc = mdb_env_create(&env->mdbEnv);
      if (rc != 0) {
         result = LMDB_WRAPPER_ERR_ENV_CREATE;
      } /* if env_create failed */
   } /* if result success */

   /* Configure map size */
   if (result == LMDB_WRAPPER_SUCCESS) {
      mapSize = (config->mapSize > 0U) ? config->mapSize : DEFAULT_MAP_SIZE;
      rc = mdb_env_set_mapsize(env->mdbEnv, mapSize);
      if (rc != 0) {
         result = LMDB_WRAPPER_ERR_ENV_CREATE;
      } /* if set_mapsize failed */
   } /* if result success */

   /* Configure max databases */
   if (result == LMDB_WRAPPER_SUCCESS) {
      maxDbs = (config->maxDbs > 0U) ? config->maxDbs : DEFAULT_MAX_DBS;
      rc = mdb_env_set_maxdbs(env->mdbEnv, (MDB_dbi)maxDbs);
      if (rc != 0) {
         result = LMDB_WRAPPER_ERR_ENV_CREATE;
      } /* if set_maxdbs failed */
   } /* if result success */

   /* Open the environment */
   if (result == LMDB_WRAPPER_SUCCESS) {
      rc = mdb_env_open(env->mdbEnv, config->path, 0, 0664);
      if (rc != 0) {
         result = LMDB_WRAPPER_ERR_ENV_OPEN;
      } /* if env_open failed */
   } /* if result success */

   /* Open default database */
   if (result == LMDB_WRAPPER_SUCCESS) {
      result = openDefaultDb(env);
   } /* if result success */

   /* Set output on success, cleanup on failure */
   if (result == LMDB_WRAPPER_SUCCESS) {
      *envOut = env;
   } else {
      if (env != NULL) {
         if (env->mdbEnv != NULL) {
            mdb_env_close(env->mdbEnv);
         } /* if mdbEnv */
         free(env);
      } /* if env */
   } /* if result */

   return result;
} /* lmdbWrapperEnvCreate */

void lmdbWrapperEnvClose(lmdbWrapperEnv_t *env)
{
   if (env != NULL) {
      if (env->mdbEnv != NULL) {
         if (env->dbiOpen != 0) {
            mdb_dbi_close(env->mdbEnv, env->mdbDbi);
         } /* if dbiOpen */
         mdb_env_close(env->mdbEnv);
      } /* if mdbEnv */
      free(env);
   } /* if env */
} /* lmdbWrapperEnvClose */

lmdbWrapperErr_t lmdbWrapperPut(
   lmdbWrapperEnv_t *env,
   const void *key,
   size_t keySize,
   const void *val,
   size_t valSize)
{
   lmdbWrapperErr_t result = LMDB_WRAPPER_SUCCESS;
   MDB_txn *txn = NULL;
   MDB_val mdbKey = {0};
   MDB_val mdbVal = {0};
   int rc = 0;

   /* MISRA C: Validate all input parameters */
   if (env == NULL) {
      result = LMDB_WRAPPER_ERR_NULL_PARAM;
   } else if (key == NULL) {
      result = LMDB_WRAPPER_ERR_NULL_PARAM;
   } else if (val == NULL) {
      result = LMDB_WRAPPER_ERR_NULL_PARAM;
   } else if (keySize == 0U) {
      result = LMDB_WRAPPER_ERR_INVALID_PARAM;
   } else if (valSize == 0U) {
      result = LMDB_WRAPPER_ERR_INVALID_PARAM;
   } else if (env->dbiOpen == 0) {
      result = LMDB_WRAPPER_ERR_INTERNAL;
   } /* if parameter validation */

   /* Begin write transaction */
   if (result == LMDB_WRAPPER_SUCCESS) {
      rc = mdb_txn_begin(env->mdbEnv, NULL, 0, &txn);
      if (rc != 0) {
         result = LMDB_WRAPPER_ERR_TXN_BEGIN;
      } /* if txn_begin failed */
   } /* if result success */

   /* Put record */
   if (result == LMDB_WRAPPER_SUCCESS) {
      mdbKey.mv_size = keySize;
      mdbKey.mv_data = (void *)(uintptr_t)key;
      mdbVal.mv_size = valSize;
      mdbVal.mv_data = (void *)(uintptr_t)val;

      rc = mdb_put(txn, env->mdbDbi, &mdbKey, &mdbVal, 0);
      if (rc != 0) {
         mdb_txn_abort(txn);
         txn = NULL;
         if (rc == MDB_MAP_FULL) {
            result = LMDB_WRAPPER_ERR_MAP_FULL;
         } else {
            result = LMDB_WRAPPER_ERR_PUT;
         } /* if map_full */
      } /* if put failed */
   } /* if result success */

   /* Commit transaction */
   if (result == LMDB_WRAPPER_SUCCESS) {
      rc = mdb_txn_commit(txn);
      if (rc != 0) {
         result = LMDB_WRAPPER_ERR_TXN_COMMIT;
      } /* if txn_commit failed */
   } /* if result success */

   return result;
} /* lmdbWrapperPut */

lmdbWrapperErr_t lmdbWrapperGet(
   lmdbWrapperEnv_t *env,
   const void *key,
   size_t keySize,
   void *valBuf,
   size_t valCapacity,
   size_t *valSizeOut)
{
   lmdbWrapperErr_t result = LMDB_WRAPPER_SUCCESS;
   MDB_txn *txn = NULL;
   MDB_val mdbKey = {0};
   MDB_val mdbVal = {0};
   int rc = 0;

   /* MISRA C: Validate all input parameters */
   if (env == NULL) {
      result = LMDB_WRAPPER_ERR_NULL_PARAM;
   } else if (key == NULL) {
      result = LMDB_WRAPPER_ERR_NULL_PARAM;
   } else if (valSizeOut == NULL) {
      result = LMDB_WRAPPER_ERR_NULL_PARAM;
   } else if (keySize == 0U) {
      result = LMDB_WRAPPER_ERR_INVALID_PARAM;
   } else if ((valBuf == NULL) && (valCapacity > 0U)) {
      result = LMDB_WRAPPER_ERR_NULL_PARAM;
   } else if (env->dbiOpen == 0) {
      result = LMDB_WRAPPER_ERR_INTERNAL;
   } /* if parameter validation */

   /* Initialize outputs */
   if (result == LMDB_WRAPPER_SUCCESS) {
      *valSizeOut = 0U;
   } /* if result success */

   /* Begin read-only transaction */
   if (result == LMDB_WRAPPER_SUCCESS) {
      rc = mdb_txn_begin(env->mdbEnv, NULL, MDB_RDONLY, &txn);
      if (rc != 0) {
         result = LMDB_WRAPPER_ERR_TXN_BEGIN;
      } /* if txn_begin failed */
   } /* if result success */

   /* Get record */
   if (result == LMDB_WRAPPER_SUCCESS) {
      mdbKey.mv_size = keySize;
      mdbKey.mv_data = (void *)(uintptr_t)key;

      rc = mdb_get(txn, env->mdbDbi, &mdbKey, &mdbVal);
      if (rc != 0) {
         if (rc == MDB_NOTFOUND) {
            result = LMDB_WRAPPER_ERR_NOT_FOUND;
         } else {
            result = LMDB_WRAPPER_ERR_GET;
         } /* if not_found */
      } /* if get failed */
   } /* if result success */

   /* Copy results */
   if (result == LMDB_WRAPPER_SUCCESS) {
      *valSizeOut = mdbVal.mv_size;

      if (valCapacity > 0U) {
         if (valCapacity < mdbVal.mv_size) {
            result = LMDB_WRAPPER_ERR_BUFFER_TOO_SMALL;
         } else {
            (void)memcpy(valBuf, mdbVal.mv_data, mdbVal.mv_size);
         } /* if buffer too small */
      } /* if copy requested */
   } /* if result success */

   /* Abort read-only transaction */
   if (txn != NULL) {
      mdb_txn_abort(txn);
   } /* if txn */

   return result;
} /* lmdbWrapperGet */

lmdbWrapperErr_t lmdbWrapperDel(
   lmdbWrapperEnv_t *env,
   const void *key,
   size_t keySize)
{
   lmdbWrapperErr_t result = LMDB_WRAPPER_SUCCESS;
   MDB_txn *txn = NULL;
   MDB_val mdbKey = {0};
   int rc = 0;

   /* MISRA C: Validate all input parameters */
   if (env == NULL) {
      result = LMDB_WRAPPER_ERR_NULL_PARAM;
   } else if (key == NULL) {
      result = LMDB_WRAPPER_ERR_NULL_PARAM;
   } else if (keySize == 0U) {
      result = LMDB_WRAPPER_ERR_INVALID_PARAM;
   } else if (env->dbiOpen == 0) {
      result = LMDB_WRAPPER_ERR_INTERNAL;
   } /* if parameter validation */

   /* Begin write transaction */
   if (result == LMDB_WRAPPER_SUCCESS) {
      rc = mdb_txn_begin(env->mdbEnv, NULL, 0, &txn);
      if (rc != 0) {
         result = LMDB_WRAPPER_ERR_TXN_BEGIN;
      } /* if txn_begin failed */
   } /* if result success */

   /* Delete record */
   if (result == LMDB_WRAPPER_SUCCESS) {
      mdbKey.mv_size = keySize;
      mdbKey.mv_data = (void *)(uintptr_t)key;

      rc = mdb_del(txn, env->mdbDbi, &mdbKey, NULL);
      if (rc != 0) {
         mdb_txn_abort(txn);
         txn = NULL;
         if (rc == MDB_NOTFOUND) {
            result = LMDB_WRAPPER_ERR_NOT_FOUND;
         } else {
            result = LMDB_WRAPPER_ERR_DEL;
         } /* if not_found */
      } /* if del failed */
   } /* if result success */

   /* Commit transaction */
   if (result == LMDB_WRAPPER_SUCCESS) {
      rc = mdb_txn_commit(txn);
      if (rc != 0) {
         result = LMDB_WRAPPER_ERR_TXN_COMMIT;
      } /* if txn_commit failed */
   } /* if result success */

   return result;
} /* lmdbWrapperDel */

const char *lmdbWrapperStrerror(lmdbWrapperErr_t err)
{
   const char *msg = "Unknown error";

   switch (err) {
      case LMDB_WRAPPER_SUCCESS:
         msg = "Success";
         break;
      case LMDB_WRAPPER_ERR_NULL_PARAM:
         msg = "Null parameter";
         break;
      case LMDB_WRAPPER_ERR_INVALID_PARAM:
         msg = "Invalid parameter";
         break;
      case LMDB_WRAPPER_ERR_ENV_CREATE:
         msg = "Failed to create environment";
         break;
      case LMDB_WRAPPER_ERR_ENV_OPEN:
         msg = "Failed to open environment";
         break;
      case LMDB_WRAPPER_ERR_TXN_BEGIN:
         msg = "Failed to begin transaction";
         break;
      case LMDB_WRAPPER_ERR_DBI_OPEN:
         msg = "Failed to open database";
         break;
      case LMDB_WRAPPER_ERR_PUT:
         msg = "Failed to write record";
         break;
      case LMDB_WRAPPER_ERR_GET:
         msg = "Failed to read record";
         break;
      case LMDB_WRAPPER_ERR_DEL:
         msg = "Failed to delete record";
         break;
      case LMDB_WRAPPER_ERR_TXN_COMMIT:
         msg = "Failed to commit transaction";
         break;
      case LMDB_WRAPPER_ERR_NOT_FOUND:
         msg = "Record not found";
         break;
      case LMDB_WRAPPER_ERR_MAP_FULL:
         msg = "Database map is full";
         break;
      case LMDB_WRAPPER_ERR_BUFFER_TOO_SMALL:
         msg = "Output buffer is too small";
         break;
      case LMDB_WRAPPER_ERR_INTERNAL:
         msg = "Internal error";
         break;
      default:
         /* msg already set to "Unknown error" */
         break;
   } /* switch err */

   return msg;
} /* lmdbWrapperStrerror */
