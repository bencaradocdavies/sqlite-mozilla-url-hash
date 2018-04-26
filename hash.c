/*
 * SQLite extension to calculate the url_hash used in places.sqlite by Mozilla Firefox 50 and later.
 *
 * The Mozilla Firefox hash implementation can be found in these files:
 * https://dxr.mozilla.org/mozilla-central/source/toolkit/components/places/Database.cpp
 * https://dxr.mozilla.org/mozilla-central/source/toolkit/components/places/Helpers.cpp
 * https://dxr.mozilla.org/mozilla-central/source/mfbt/HashFunctions.h
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "sqlite3ext.h"
SQLITE_EXTENSION_INIT1
#include <assert.h>
#include <stdint.h>

static const uint32_t golden_ratio = 0x9E3779B9U;

static uint32_t rotate_left_5(uint32_t value) {
    return (value << 5) | (value >> 27);
}

static uint32_t add_to_hash(uint32_t hash, uint32_t value) {
    return golden_ratio * (rotate_left_5(hash) ^ value);
}

/* Calculate the 32-bit hash of the first len bytes of url. */
static uint64_t hash_simple(const unsigned char *url, int len) {
    uint32_t hash = 0;
    for (int i = 0; i < len; i++) {
        hash = add_to_hash(hash, url[i]);
    }
    return hash;
}

/*
 * Calculate the 48-bit hash of the first len bytes of url. This hash includes a
 * 16-bit hash of any prefix (characters before the first ":").
 */
uint64_t hash(const unsigned char *url, int len) {
    int prefix = -1;
    for (int i = 0; i < len; i++) {
        if (url[i] == ':') {
            prefix = i;
            break;
        }
    }
    return ((hash_simple(url, prefix) & 0x0000FFFF) << 32) + hash_simple(url, len);
}

static void sqlite_hash(sqlite3_context *context, int argc, sqlite3_value **argv){
    assert(argc == 1);
    if(sqlite3_value_type(argv[0]) == SQLITE_NULL) {
        return;
    }
    const unsigned char *url = sqlite3_value_text(argv[0]);
    int len = sqlite3_value_bytes(argv[0]);
    sqlite3_result_int64(context, hash(url, len));
}

#ifdef _WIN32
__declspec(dllexport)
#endif
int sqlite3_sqlitemozillaurlhash_init(sqlite3 *db, char **pzErrMsg, const sqlite3_api_routines *pApi){
    SQLITE_EXTENSION_INIT2(pApi);
    return sqlite3_create_function(db, "hash", 1, SQLITE_UTF8, 0, sqlite_hash, 0, 0);
}
