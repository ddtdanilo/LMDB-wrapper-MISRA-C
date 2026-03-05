/**
 * @file basic_usage.c
 * @brief Demonstrates all LMDB wrapper operations: create, put, get, delete.
 *
 * Build and run:
 *   cmake -B build -DCMAKE_BUILD_TYPE=Debug
 *   cmake --build build
 *   ./build/example_basic_usage
 */

#include "lmdb_wrapper.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define EXAMPLE_DB_PATH "/tmp/lmdb_wrapper_example"
#define EXAMPLE_MAP_SIZE ((size_t)(1024U * 1024U))  /* 1 MB */

/**
 * @brief Print result of an operation, exit on failure.
 */
static void check(lmdbWrapperErr_t err, const char *operation)
{
   if (err != LMDB_WRAPPER_SUCCESS) {
      (void)fprintf(stderr, "FAILED: %s -> %s\n",
                    operation, lmdbWrapperStrerror(err));
      exit(1);
   } /* if err */
   (void)printf("  OK: %s\n", operation);
} /* check */

int main(void)
{
   lmdbWrapperEnv_t *env = NULL;
   lmdbWrapperErr_t err = LMDB_WRAPPER_SUCCESS;
   void *valOut = NULL;
   size_t valSize = 0U;

   /* Create database directory */
   (void)mkdir(EXAMPLE_DB_PATH, 0755);

   (void)printf("\n=== LMDB Wrapper - Basic Usage Example ===\n\n");

   /* --------------------------------------------------------
    * 1. Create environment
    * -------------------------------------------------------- */
   (void)printf("[1] Create environment\n");
   lmdbWrapperConfig_t config;
   config.path = EXAMPLE_DB_PATH;
   config.mapSize = EXAMPLE_MAP_SIZE;
   config.maxDbs = 0U;  /* default */

   err = lmdbWrapperEnvCreate(&env, &config);
   check(err, "lmdbWrapperEnvCreate");

   /* --------------------------------------------------------
    * 2. Store records (put)
    * -------------------------------------------------------- */
   (void)printf("\n[2] Store records\n");

   const char *key1 = "user:1001";
   const char *val1 = "Alice";
   err = lmdbWrapperPut(env,
                        key1, strlen(key1),
                        val1, strlen(val1));
   check(err, "put user:1001 = Alice");

   const char *key2 = "user:1002";
   const char *val2 = "Bob";
   err = lmdbWrapperPut(env,
                        key2, strlen(key2),
                        val2, strlen(val2));
   check(err, "put user:1002 = Bob");

   const char *key3 = "user:1003";
   const char *val3 = "Charlie";
   err = lmdbWrapperPut(env,
                        key3, strlen(key3),
                        val3, strlen(val3));
   check(err, "put user:1003 = Charlie");

   /* --------------------------------------------------------
    * 3. Read records (get)
    * -------------------------------------------------------- */
   (void)printf("\n[3] Read records\n");

   err = lmdbWrapperGet(env,
                        key1, strlen(key1),
                        &valOut, &valSize);
   check(err, "get user:1001");
   (void)printf("       -> value: \"%.*s\" (size: %zu)\n",
                (int)valSize, (const char *)valOut, valSize);

   err = lmdbWrapperGet(env,
                        key2, strlen(key2),
                        &valOut, &valSize);
   check(err, "get user:1002");
   (void)printf("       -> value: \"%.*s\" (size: %zu)\n",
                (int)valSize, (const char *)valOut, valSize);

   /* --------------------------------------------------------
    * 4. Update a record (overwrite)
    * -------------------------------------------------------- */
   (void)printf("\n[4] Update record\n");

   const char *val1Updated = "Alice (admin)";
   err = lmdbWrapperPut(env,
                        key1, strlen(key1),
                        val1Updated, strlen(val1Updated));
   check(err, "put user:1001 = Alice (admin)");

   err = lmdbWrapperGet(env,
                        key1, strlen(key1),
                        &valOut, &valSize);
   check(err, "get user:1001 (updated)");
   (void)printf("       -> value: \"%.*s\"\n",
                (int)valSize, (const char *)valOut);

   /* --------------------------------------------------------
    * 5. Delete a record
    * -------------------------------------------------------- */
   (void)printf("\n[5] Delete record\n");

   err = lmdbWrapperDel(env, key3, strlen(key3));
   check(err, "del user:1003");

   /* Verify it's gone */
   err = lmdbWrapperGet(env,
                        key3, strlen(key3),
                        &valOut, &valSize);
   if (err == LMDB_WRAPPER_ERR_NOT_FOUND) {
      (void)printf("  OK: user:1003 confirmed deleted (NOT_FOUND)\n");
   } else {
      (void)fprintf(stderr, "  FAIL: expected NOT_FOUND, got %s\n",
                    lmdbWrapperStrerror(err));
      lmdbWrapperEnvClose(env);
      return 1;
   } /* if not_found */

   /* --------------------------------------------------------
    * 6. Error handling demo
    * -------------------------------------------------------- */
   (void)printf("\n[6] Error handling\n");

   /* Try to get a key that doesn't exist */
   const char *missingKey = "user:9999";
   err = lmdbWrapperGet(env,
                        missingKey, strlen(missingKey),
                        &valOut, &valSize);
   (void)printf("  get non-existent key -> %s (code: %d)\n",
                lmdbWrapperStrerror(err), (int)err);

   /* Try null parameter */
   err = lmdbWrapperPut(NULL, key1, strlen(key1), val1, strlen(val1));
   (void)printf("  put with NULL env    -> %s (code: %d)\n",
                lmdbWrapperStrerror(err), (int)err);

   /* --------------------------------------------------------
    * 7. Reopen database (persistence check)
    * -------------------------------------------------------- */
   (void)printf("\n[7] Persistence check (close and reopen)\n");

   lmdbWrapperEnvClose(env);
   (void)printf("  OK: environment closed\n");

   env = NULL;
   err = lmdbWrapperEnvCreate(&env, &config);
   check(err, "reopen environment");

   err = lmdbWrapperGet(env,
                        key1, strlen(key1),
                        &valOut, &valSize);
   check(err, "get user:1001 after reopen");
   (void)printf("       -> value: \"%.*s\" (persisted!)\n",
                (int)valSize, (const char *)valOut);

   err = lmdbWrapperGet(env,
                        key2, strlen(key2),
                        &valOut, &valSize);
   check(err, "get user:1002 after reopen");
   (void)printf("       -> value: \"%.*s\" (persisted!)\n",
                (int)valSize, (const char *)valOut);

   /* --------------------------------------------------------
    * Cleanup
    * -------------------------------------------------------- */
   (void)printf("\n[Cleanup]\n");
   lmdbWrapperEnvClose(env);
   (void)printf("  OK: environment closed\n");

   (void)printf("\n=== All operations completed successfully ===\n\n");

   return 0;
} /* main */
