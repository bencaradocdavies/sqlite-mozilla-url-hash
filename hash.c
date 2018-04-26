/*
 * SQLite extension to calculate the url_hash used in places.sqlite by Mozilla Firefox 50 and later.
 *
 * The Mozilla Firefox hash implementation can be found in these file:
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

static uint64_t hash(const unsigned char *bytes, int start, int end) {
    uint32_t hash = 0;
    for (int i = start; i <= end; i++) {
        hash = add_to_hash(hash, bytes[i]);
    }
    return hash;
}

static void mozilla_url_hash(sqlite3_context *context, int argc, sqlite3_value **argv){
    assert(argc == 1);
    if(sqlite3_value_type(argv[0]) == SQLITE_NULL) {
        return;
    }
    int len = sqlite3_value_bytes(argv[0]);
    const unsigned char *url = sqlite3_value_text(argv[0]);
    int i_colon = -1;
    for (int i = 0; i < len; i++) {
        if (url[i] == ':') {
            i_colon = i;
            break;
        }
    }
    if (i_colon < 0) {
        return;
    }
    sqlite3_result_int64(context, ((hash(url, 0, i_colon - 1) & 0x0000FFFF) << 32) + hash(url, 0, len - 1));
}

#ifdef _WIN32
__declspec(dllexport)
#endif
int sqlite3_sqlitemozillaurlhash_init(sqlite3 *db, char **pzErrMsg, const sqlite3_api_routines *pApi){
    SQLITE_EXTENSION_INIT2(pApi);
    return sqlite3_create_function(db, "hash", 1, SQLITE_UTF8, 0, mozilla_url_hash, 0, 0);
}
