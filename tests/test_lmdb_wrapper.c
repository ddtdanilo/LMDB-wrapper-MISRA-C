/**
 * @file test_lmdb_wrapper.c
 * @brief Unit tests for the LMDB wrapper using CMocka framework.
 *
 * Test coverage:
 *   - Create a new database environment
 *   - Open an existing database
 *   - Create records (put)
 *   - Read records (get)
 *   - Delete records (del)
 *   - Update (overwrite) records
 *   - Null parameter validation
 *   - Invalid parameter validation
 *   - Record not found handling
 *   - Error string conversion
 */

#define _XOPEN_SOURCE 500

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "lmdb_wrapper.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ftw.h>

/* ---------- Helpers ---------- */

static const char *TEST_DIR = "./test_db_env";

static int removeCallback(const char *fpath, const struct stat *sb,
                           int typeflag, struct FTW *ftwbuf)
{
   (void)sb;
   (void)typeflag;
   (void)ftwbuf;
   return remove(fpath);
} /* removeCallback */

static void cleanupTestDir(void)
{
   (void)nftw(TEST_DIR, removeCallback, 64, FTW_DEPTH | FTW_PHYS);
} /* cleanupTestDir */

static void setupTestDir(void)
{
   cleanupTestDir();
   (void)mkdir(TEST_DIR, 0755);
} /* setupTestDir */

/* ---------- CMocka setup / teardown ---------- */

static int testSetup(void **state)
{
   (void)state;
   setupTestDir();
   return 0;
} /* testSetup */

static int testTeardown(void **state)
{
   (void)state;
   cleanupTestDir();
   return 0;
} /* testTeardown */

/* ---------- Tests: Null parameter validation ---------- */

static void testEnvCreateNullEnvOut(void **state)
{
   (void)state;
   lmdbWrapperConfig_t config = { .path = TEST_DIR, .mapSize = 0, .maxDbs = 0 };
   lmdbWrapperErr_t err = lmdbWrapperEnvCreate(NULL, &config);
   assert_int_equal(err, LMDB_WRAPPER_ERR_NULL_PARAM);
} /* testEnvCreateNullEnvOut */

static void testEnvCreateNullConfig(void **state)
{
   (void)state;
   lmdbWrapperEnv_t *env = NULL;
   lmdbWrapperErr_t err = lmdbWrapperEnvCreate(&env, NULL);
   assert_int_equal(err, LMDB_WRAPPER_ERR_NULL_PARAM);
} /* testEnvCreateNullConfig */

static void testEnvCreateNullPath(void **state)
{
   (void)state;
   lmdbWrapperEnv_t *env = NULL;
   lmdbWrapperConfig_t config = { .path = NULL, .mapSize = 0, .maxDbs = 0 };
   lmdbWrapperErr_t err = lmdbWrapperEnvCreate(&env, &config);
   assert_int_equal(err, LMDB_WRAPPER_ERR_NULL_PARAM);
} /* testEnvCreateNullPath */

static void testEnvCreateEmptyPath(void **state)
{
   (void)state;
   lmdbWrapperEnv_t *env = NULL;
   lmdbWrapperConfig_t config = { .path = "", .mapSize = 0, .maxDbs = 0 };
   lmdbWrapperErr_t err = lmdbWrapperEnvCreate(&env, &config);
   assert_int_equal(err, LMDB_WRAPPER_ERR_INVALID_PARAM);
} /* testEnvCreateEmptyPath */

static void testPutNullEnv(void **state)
{
   (void)state;
   const char *key = "k";
   const char *val = "v";
   lmdbWrapperErr_t err = lmdbWrapperPut(NULL, key, 1, val, 1);
   assert_int_equal(err, LMDB_WRAPPER_ERR_NULL_PARAM);
} /* testPutNullEnv */

static void testPutNullKey(void **state)
{
   (void)state;
   lmdbWrapperEnv_t *env = NULL;
   lmdbWrapperConfig_t config = { .path = TEST_DIR, .mapSize = 0, .maxDbs = 0 };
   lmdbWrapperErr_t err = lmdbWrapperEnvCreate(&env, &config);
   assert_int_equal(err, LMDB_WRAPPER_SUCCESS);

   const char *val = "v";
   err = lmdbWrapperPut(env, NULL, 1, val, 1);
   assert_int_equal(err, LMDB_WRAPPER_ERR_NULL_PARAM);

   lmdbWrapperEnvClose(env);
} /* testPutNullKey */

static void testPutNullVal(void **state)
{
   (void)state;
   lmdbWrapperEnv_t *env = NULL;
   lmdbWrapperConfig_t config = { .path = TEST_DIR, .mapSize = 0, .maxDbs = 0 };
   lmdbWrapperErr_t err = lmdbWrapperEnvCreate(&env, &config);
   assert_int_equal(err, LMDB_WRAPPER_SUCCESS);

   const char *key = "k";
   err = lmdbWrapperPut(env, key, 1, NULL, 1);
   assert_int_equal(err, LMDB_WRAPPER_ERR_NULL_PARAM);

   lmdbWrapperEnvClose(env);
} /* testPutNullVal */

static void testGetNullEnv(void **state)
{
   (void)state;
   const char *key = "k";
   char val[2] = {0};
   size_t valSize = 0;
   lmdbWrapperErr_t err = lmdbWrapperGet(NULL, key, 1, val, sizeof(val), &valSize);
   assert_int_equal(err, LMDB_WRAPPER_ERR_NULL_PARAM);
} /* testGetNullEnv */

static void testGetNullValBufWithCapacity(void **state)
{
   (void)state;
   lmdbWrapperEnv_t *env = NULL;
   lmdbWrapperConfig_t config = { .path = TEST_DIR, .mapSize = 0, .maxDbs = 0 };
   lmdbWrapperErr_t err = lmdbWrapperEnvCreate(&env, &config);
   assert_int_equal(err, LMDB_WRAPPER_SUCCESS);

   const char *key = "key";
   const char *val = "value";
   size_t valSize = 0;
   err = lmdbWrapperPut(env, key, strlen(key), val, strlen(val));
   assert_int_equal(err, LMDB_WRAPPER_SUCCESS);

   err = lmdbWrapperGet(env, key, strlen(key), NULL, 1U, &valSize);
   assert_int_equal(err, LMDB_WRAPPER_ERR_NULL_PARAM);

   lmdbWrapperEnvClose(env);
} /* testGetNullValBufWithCapacity */

static void testGetNullValSizeOut(void **state)
{
   (void)state;
   lmdbWrapperEnv_t *env = NULL;
   lmdbWrapperConfig_t config = { .path = TEST_DIR, .mapSize = 0, .maxDbs = 0 };
   lmdbWrapperErr_t err = lmdbWrapperEnvCreate(&env, &config);
   assert_int_equal(err, LMDB_WRAPPER_SUCCESS);

   const char *key = "k";
   char val[2] = {0};
   err = lmdbWrapperGet(env, key, 1, val, sizeof(val), NULL);
   assert_int_equal(err, LMDB_WRAPPER_ERR_NULL_PARAM);

   lmdbWrapperEnvClose(env);
} /* testGetNullValSizeOut */

static void testDelNullEnv(void **state)
{
   (void)state;
   const char *key = "k";
   lmdbWrapperErr_t err = lmdbWrapperDel(NULL, key, 1);
   assert_int_equal(err, LMDB_WRAPPER_ERR_NULL_PARAM);
} /* testDelNullEnv */

static void testDelNullKey(void **state)
{
   (void)state;
   lmdbWrapperEnv_t *env = NULL;
   lmdbWrapperConfig_t config = { .path = TEST_DIR, .mapSize = 0, .maxDbs = 0 };
   lmdbWrapperErr_t err = lmdbWrapperEnvCreate(&env, &config);
   assert_int_equal(err, LMDB_WRAPPER_SUCCESS);

   err = lmdbWrapperDel(env, NULL, 1);
   assert_int_equal(err, LMDB_WRAPPER_ERR_NULL_PARAM);

   lmdbWrapperEnvClose(env);
} /* testDelNullKey */

/* ---------- Tests: Invalid parameter validation ---------- */

static void testPutZeroKeySize(void **state)
{
   (void)state;
   lmdbWrapperEnv_t *env = NULL;
   lmdbWrapperConfig_t config = { .path = TEST_DIR, .mapSize = 0, .maxDbs = 0 };
   lmdbWrapperErr_t err = lmdbWrapperEnvCreate(&env, &config);
   assert_int_equal(err, LMDB_WRAPPER_SUCCESS);

   const char *key = "k";
   const char *val = "v";
   err = lmdbWrapperPut(env, key, 0, val, 1);
   assert_int_equal(err, LMDB_WRAPPER_ERR_INVALID_PARAM);

   lmdbWrapperEnvClose(env);
} /* testPutZeroKeySize */

static void testPutZeroValSize(void **state)
{
   (void)state;
   lmdbWrapperEnv_t *env = NULL;
   lmdbWrapperConfig_t config = { .path = TEST_DIR, .mapSize = 0, .maxDbs = 0 };
   lmdbWrapperErr_t err = lmdbWrapperEnvCreate(&env, &config);
   assert_int_equal(err, LMDB_WRAPPER_SUCCESS);

   const char *key = "k";
   const char *val = "v";
   err = lmdbWrapperPut(env, key, 1, val, 0);
   assert_int_equal(err, LMDB_WRAPPER_ERR_INVALID_PARAM);

   lmdbWrapperEnvClose(env);
} /* testPutZeroValSize */

static void testGetZeroKeySize(void **state)
{
   (void)state;
   lmdbWrapperEnv_t *env = NULL;
   lmdbWrapperConfig_t config = { .path = TEST_DIR, .mapSize = 0, .maxDbs = 0 };
   lmdbWrapperErr_t err = lmdbWrapperEnvCreate(&env, &config);
   assert_int_equal(err, LMDB_WRAPPER_SUCCESS);

   const char *key = "k";
   char val[2] = {0};
   size_t valSize = 0;
   err = lmdbWrapperGet(env, key, 0, val, sizeof(val), &valSize);
   assert_int_equal(err, LMDB_WRAPPER_ERR_INVALID_PARAM);

   lmdbWrapperEnvClose(env);
} /* testGetZeroKeySize */

static void testDelZeroKeySize(void **state)
{
   (void)state;
   lmdbWrapperEnv_t *env = NULL;
   lmdbWrapperConfig_t config = { .path = TEST_DIR, .mapSize = 0, .maxDbs = 0 };
   lmdbWrapperErr_t err = lmdbWrapperEnvCreate(&env, &config);
   assert_int_equal(err, LMDB_WRAPPER_SUCCESS);

   const char *key = "k";
   err = lmdbWrapperDel(env, key, 0);
   assert_int_equal(err, LMDB_WRAPPER_ERR_INVALID_PARAM);

   lmdbWrapperEnvClose(env);
} /* testDelZeroKeySize */

/* ---------- Tests: Create database ---------- */

static void testCreateDatabase(void **state)
{
   (void)state;
   lmdbWrapperEnv_t *env = NULL;
   lmdbWrapperConfig_t config = { .path = TEST_DIR, .mapSize = 0, .maxDbs = 0 };

   lmdbWrapperErr_t err = lmdbWrapperEnvCreate(&env, &config);
   assert_int_equal(err, LMDB_WRAPPER_SUCCESS);
   assert_non_null(env);

   lmdbWrapperEnvClose(env);
} /* testCreateDatabase */

/* ---------- Tests: Open existing database ---------- */

static void testOpenExistingDatabase(void **state)
{
   (void)state;
   lmdbWrapperConfig_t config = { .path = TEST_DIR, .mapSize = 0, .maxDbs = 0 };

   /* Create and close */
   lmdbWrapperEnv_t *env1 = NULL;
   lmdbWrapperErr_t err = lmdbWrapperEnvCreate(&env1, &config);
   assert_int_equal(err, LMDB_WRAPPER_SUCCESS);

   /* Write a record so the DB has content */
   const char *key = "persistKey";
   const char *val = "persistVal";
   err = lmdbWrapperPut(env1, key, strlen(key), val, strlen(val));
   assert_int_equal(err, LMDB_WRAPPER_SUCCESS);
   lmdbWrapperEnvClose(env1);

   /* Re-open the same database */
   lmdbWrapperEnv_t *env2 = NULL;
   err = lmdbWrapperEnvCreate(&env2, &config);
   assert_int_equal(err, LMDB_WRAPPER_SUCCESS);

   /* Verify the record persisted */
   char readVal[32] = {0};
   size_t readSize = 0;
   err = lmdbWrapperGet(env2, key, strlen(key), readVal, sizeof(readVal), &readSize);
   assert_int_equal(err, LMDB_WRAPPER_SUCCESS);
   assert_true(readSize == strlen(val));
   assert_memory_equal(readVal, val, readSize);

   lmdbWrapperEnvClose(env2);
} /* testOpenExistingDatabase */

/* ---------- Tests: Put and Get records ---------- */

static void testPutAndGetRecord(void **state)
{
   (void)state;
   lmdbWrapperEnv_t *env = NULL;
   lmdbWrapperConfig_t config = { .path = TEST_DIR, .mapSize = 0, .maxDbs = 0 };
   lmdbWrapperErr_t err = lmdbWrapperEnvCreate(&env, &config);
   assert_int_equal(err, LMDB_WRAPPER_SUCCESS);

   /* Put a record */
   const char *key = "hello";
   const char *val = "world";
   err = lmdbWrapperPut(env, key, strlen(key), val, strlen(val));
   assert_int_equal(err, LMDB_WRAPPER_SUCCESS);

   /* Get the record back */
   char readVal[16] = {0};
   size_t readSize = 0;
   err = lmdbWrapperGet(env, key, strlen(key), readVal, sizeof(readVal), &readSize);
   assert_int_equal(err, LMDB_WRAPPER_SUCCESS);
   assert_true(readSize == strlen(val));
   assert_memory_equal(readVal, val, readSize);

   lmdbWrapperEnvClose(env);
} /* testPutAndGetRecord */

static void testPutMultipleRecords(void **state)
{
   (void)state;
   lmdbWrapperEnv_t *env = NULL;
   lmdbWrapperConfig_t config = { .path = TEST_DIR, .mapSize = 0, .maxDbs = 0 };
   lmdbWrapperErr_t err = lmdbWrapperEnvCreate(&env, &config);
   assert_int_equal(err, LMDB_WRAPPER_SUCCESS);

   /* Put several records */
   const char *keys[] = {"key1", "key2", "key3"};
   const char *vals[] = {"val1", "val2", "val3"};
   size_t count = sizeof(keys) / sizeof(keys[0]);

   for (size_t i = 0; i < count; i++) {
      err = lmdbWrapperPut(env, keys[i], strlen(keys[i]),
                           vals[i], strlen(vals[i]));
      assert_int_equal(err, LMDB_WRAPPER_SUCCESS);
   } /* for put */

   /* Read them all back */
   for (size_t i = 0; i < count; i++) {
      char readVal[16] = {0};
      size_t readSize = 0;
      err = lmdbWrapperGet(env, keys[i], strlen(keys[i]),
                           readVal, sizeof(readVal), &readSize);
      assert_int_equal(err, LMDB_WRAPPER_SUCCESS);
      assert_true(readSize == strlen(vals[i]));
      assert_memory_equal(readVal, vals[i], readSize);
   } /* for get */

   lmdbWrapperEnvClose(env);
} /* testPutMultipleRecords */

/* ---------- Tests: Delete records ---------- */

static void testDeleteRecord(void **state)
{
   (void)state;
   lmdbWrapperEnv_t *env = NULL;
   lmdbWrapperConfig_t config = { .path = TEST_DIR, .mapSize = 0, .maxDbs = 0 };
   lmdbWrapperErr_t err = lmdbWrapperEnvCreate(&env, &config);
   assert_int_equal(err, LMDB_WRAPPER_SUCCESS);

   /* Put a record */
   const char *key = "deleteMe";
   const char *val = "temporary";
   err = lmdbWrapperPut(env, key, strlen(key), val, strlen(val));
   assert_int_equal(err, LMDB_WRAPPER_SUCCESS);

   /* Verify it exists */
   char readVal[16] = {0};
   size_t readSize = 0;
   err = lmdbWrapperGet(env, key, strlen(key), readVal, sizeof(readVal), &readSize);
   assert_int_equal(err, LMDB_WRAPPER_SUCCESS);

   /* Delete it */
   err = lmdbWrapperDel(env, key, strlen(key));
   assert_int_equal(err, LMDB_WRAPPER_SUCCESS);

   /* Verify it's gone */
   err = lmdbWrapperGet(env, key, strlen(key), readVal, sizeof(readVal), &readSize);
   assert_int_equal(err, LMDB_WRAPPER_ERR_NOT_FOUND);

   lmdbWrapperEnvClose(env);
} /* testDeleteRecord */

/* ---------- Tests: Update record (overwrite) ---------- */

static void testUpdateRecord(void **state)
{
   (void)state;
   lmdbWrapperEnv_t *env = NULL;
   lmdbWrapperConfig_t config = { .path = TEST_DIR, .mapSize = 0, .maxDbs = 0 };
   lmdbWrapperErr_t err = lmdbWrapperEnvCreate(&env, &config);
   assert_int_equal(err, LMDB_WRAPPER_SUCCESS);

   const char *key = "updateKey";
   const char *val1 = "original";
   const char *val2 = "updated";

   /* Put original */
   err = lmdbWrapperPut(env, key, strlen(key), val1, strlen(val1));
   assert_int_equal(err, LMDB_WRAPPER_SUCCESS);

   /* Overwrite */
   err = lmdbWrapperPut(env, key, strlen(key), val2, strlen(val2));
   assert_int_equal(err, LMDB_WRAPPER_SUCCESS);

   /* Verify updated value */
   char readVal[16] = {0};
   size_t readSize = 0;
   err = lmdbWrapperGet(env, key, strlen(key), readVal, sizeof(readVal), &readSize);
   assert_int_equal(err, LMDB_WRAPPER_SUCCESS);
   assert_true(readSize == strlen(val2));
   assert_memory_equal(readVal, val2, readSize);

   lmdbWrapperEnvClose(env);
} /* testUpdateRecord */

static void testGetSizeQuery(void **state)
{
   (void)state;
   lmdbWrapperEnv_t *env = NULL;
   lmdbWrapperConfig_t config = { .path = TEST_DIR, .mapSize = 0, .maxDbs = 0 };
   lmdbWrapperErr_t err = lmdbWrapperEnvCreate(&env, &config);
   assert_int_equal(err, LMDB_WRAPPER_SUCCESS);

   const char *key = "queryKey";
   const char *val = "queryValue";
   size_t readSize = 0;

   err = lmdbWrapperPut(env, key, strlen(key), val, strlen(val));
   assert_int_equal(err, LMDB_WRAPPER_SUCCESS);

   err = lmdbWrapperGet(env, key, strlen(key), NULL, 0U, &readSize);
   assert_int_equal(err, LMDB_WRAPPER_SUCCESS);
   assert_true(readSize == strlen(val));

   lmdbWrapperEnvClose(env);
} /* testGetSizeQuery */

static void testGetBufferTooSmall(void **state)
{
   (void)state;
   lmdbWrapperEnv_t *env = NULL;
   lmdbWrapperConfig_t config = { .path = TEST_DIR, .mapSize = 0, .maxDbs = 0 };
   lmdbWrapperErr_t err = lmdbWrapperEnvCreate(&env, &config);
   assert_int_equal(err, LMDB_WRAPPER_SUCCESS);

   const char *key = "smallBufferKey";
   const char *val = "0123456789";
   char readVal[4] = {'X', 'X', 'X', 'X'};
   char expected[4] = {'X', 'X', 'X', 'X'};
   size_t readSize = 0;

   err = lmdbWrapperPut(env, key, strlen(key), val, strlen(val));
   assert_int_equal(err, LMDB_WRAPPER_SUCCESS);

   err = lmdbWrapperGet(env, key, strlen(key), readVal, sizeof(readVal), &readSize);
   assert_int_equal(err, LMDB_WRAPPER_ERR_BUFFER_TOO_SMALL);
   assert_true(readSize == strlen(val));
   assert_memory_equal(readVal, expected, sizeof(readVal));

   lmdbWrapperEnvClose(env);
} /* testGetBufferTooSmall */

static void testGetCopiesDataIntoCallerBuffer(void **state)
{
   (void)state;
   lmdbWrapperEnv_t *env = NULL;
   lmdbWrapperConfig_t config = { .path = TEST_DIR, .mapSize = 0, .maxDbs = 0 };
   lmdbWrapperErr_t err = lmdbWrapperEnvCreate(&env, &config);
   assert_int_equal(err, LMDB_WRAPPER_SUCCESS);

   const char *key = "copyKey";
   const char *originalVal = "original";
   const char *updatedVal = "updated";
   char readVal[16] = {0};
   size_t readSize = 0;

   err = lmdbWrapperPut(env, key, strlen(key), originalVal, strlen(originalVal));
   assert_int_equal(err, LMDB_WRAPPER_SUCCESS);

   err = lmdbWrapperGet(env, key, strlen(key), readVal, sizeof(readVal), &readSize);
   assert_int_equal(err, LMDB_WRAPPER_SUCCESS);
   assert_true(readSize == strlen(originalVal));
   assert_memory_equal(readVal, originalVal, readSize);

   err = lmdbWrapperPut(env, key, strlen(key), updatedVal, strlen(updatedVal));
   assert_int_equal(err, LMDB_WRAPPER_SUCCESS);

   assert_true(readSize == strlen(originalVal));
   assert_memory_equal(readVal, originalVal, readSize);

   lmdbWrapperEnvClose(env);
} /* testGetCopiesDataIntoCallerBuffer */

/* ---------- Tests: Not found ---------- */

static void testGetNotFound(void **state)
{
   (void)state;
   lmdbWrapperEnv_t *env = NULL;
   lmdbWrapperConfig_t config = { .path = TEST_DIR, .mapSize = 0, .maxDbs = 0 };
   lmdbWrapperErr_t err = lmdbWrapperEnvCreate(&env, &config);
   assert_int_equal(err, LMDB_WRAPPER_SUCCESS);

   const char *key = "nonexistent";
   char val[16] = {0};
   size_t valSize = 0;
   err = lmdbWrapperGet(env, key, strlen(key), val, sizeof(val), &valSize);
   assert_int_equal(err, LMDB_WRAPPER_ERR_NOT_FOUND);

   lmdbWrapperEnvClose(env);
} /* testGetNotFound */

static void testDeleteNotFound(void **state)
{
   (void)state;
   lmdbWrapperEnv_t *env = NULL;
   lmdbWrapperConfig_t config = { .path = TEST_DIR, .mapSize = 0, .maxDbs = 0 };
   lmdbWrapperErr_t err = lmdbWrapperEnvCreate(&env, &config);
   assert_int_equal(err, LMDB_WRAPPER_SUCCESS);

   const char *key = "nonexistent";
   err = lmdbWrapperDel(env, key, strlen(key));
   assert_int_equal(err, LMDB_WRAPPER_ERR_NOT_FOUND);

   lmdbWrapperEnvClose(env);
} /* testDeleteNotFound */

/* ---------- Tests: strerror ---------- */

static void testStrerror(void **state)
{
   (void)state;
   const char *msg = NULL;

   msg = lmdbWrapperStrerror(LMDB_WRAPPER_SUCCESS);
   assert_non_null(msg);
   assert_string_equal(msg, "Success");

   msg = lmdbWrapperStrerror(LMDB_WRAPPER_ERR_NULL_PARAM);
   assert_non_null(msg);
   assert_string_equal(msg, "Null parameter");

   msg = lmdbWrapperStrerror(LMDB_WRAPPER_ERR_NOT_FOUND);
   assert_non_null(msg);
   assert_string_equal(msg, "Record not found");

   /* Unknown error code */
   msg = lmdbWrapperStrerror((lmdbWrapperErr_t)9999);
   assert_non_null(msg);
   assert_string_equal(msg, "Unknown error");
} /* testStrerror */

/* ---------- Tests: envClose NULL safety ---------- */

static void testEnvCloseNull(void **state)
{
   (void)state;
   /* Should not crash */
   lmdbWrapperEnvClose(NULL);
} /* testEnvCloseNull */

/* ---------- Main ---------- */

int main(void)
{
   const struct CMUnitTest allTests[] = {
      /* Null parameter validation - no setup needed */
      cmocka_unit_test(testEnvCreateNullEnvOut),
      cmocka_unit_test(testEnvCreateNullConfig),
      cmocka_unit_test(testEnvCreateNullPath),
      cmocka_unit_test(testEnvCreateEmptyPath),
      cmocka_unit_test(testPutNullEnv),
      cmocka_unit_test(testGetNullEnv),
      cmocka_unit_test(testDelNullEnv),

      /* Null parameter validation - need setup */
      cmocka_unit_test_setup_teardown(testPutNullKey, testSetup, testTeardown),
      cmocka_unit_test_setup_teardown(testPutNullVal, testSetup, testTeardown),
      cmocka_unit_test_setup_teardown(testGetNullValBufWithCapacity, testSetup, testTeardown),
      cmocka_unit_test_setup_teardown(testGetNullValSizeOut, testSetup, testTeardown),
      cmocka_unit_test_setup_teardown(testDelNullKey, testSetup, testTeardown),

      /* Invalid parameter validation */
      cmocka_unit_test_setup_teardown(testPutZeroKeySize, testSetup, testTeardown),
      cmocka_unit_test_setup_teardown(testPutZeroValSize, testSetup, testTeardown),
      cmocka_unit_test_setup_teardown(testGetZeroKeySize, testSetup, testTeardown),
      cmocka_unit_test_setup_teardown(testDelZeroKeySize, testSetup, testTeardown),

      /* Database operations */
      cmocka_unit_test_setup_teardown(testCreateDatabase, testSetup, testTeardown),
      cmocka_unit_test_setup_teardown(testOpenExistingDatabase, testSetup, testTeardown),
      cmocka_unit_test_setup_teardown(testPutAndGetRecord, testSetup, testTeardown),
      cmocka_unit_test_setup_teardown(testPutMultipleRecords, testSetup, testTeardown),
      cmocka_unit_test_setup_teardown(testDeleteRecord, testSetup, testTeardown),
      cmocka_unit_test_setup_teardown(testUpdateRecord, testSetup, testTeardown),
      cmocka_unit_test_setup_teardown(testGetSizeQuery, testSetup, testTeardown),
      cmocka_unit_test_setup_teardown(testGetBufferTooSmall, testSetup, testTeardown),
      cmocka_unit_test_setup_teardown(testGetCopiesDataIntoCallerBuffer, testSetup, testTeardown),

      /* Not found */
      cmocka_unit_test_setup_teardown(testGetNotFound, testSetup, testTeardown),
      cmocka_unit_test_setup_teardown(testDeleteNotFound, testSetup, testTeardown),

      /* Utility */
      cmocka_unit_test(testStrerror),
      cmocka_unit_test(testEnvCloseNull),
   };

   return cmocka_run_group_tests(allTests, NULL, NULL);
} /* main */
