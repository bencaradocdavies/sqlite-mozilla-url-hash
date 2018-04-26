#include "sqlite3ext.h"
SQLITE_EXTENSION_INIT1
#include <assert.h>
#include <stdint.h>

/* https://dxr.mozilla.org/mozilla-central/source/mfbt/HashFunctions.h#64 */
static const uint32_t golden_ratio = 0x9E3779B9U;

/* https://dxr.mozilla.org/mozilla-central/source/mfbt/HashFunctions.h#70 */
static uint32_t rotate_left_5(uint32_t value) {
    return (value << 5) | (value >> 27);
}

/* https://dxr.mozilla.org/mozilla-central/source/mfbt/HashFunctions.h#76 */
static uint32_t add_to_hash(uint32_t hash, uint32_t value) {
    return golden_ratio * (rotate_left_5(hash) ^ value);
}

/* https://dxr.mozilla.org/mozilla-central/source/mfbt/HashFunctions.h#238 */
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
    /* https://dxr.mozilla.org/mozilla-central/source/toolkit/components/places/Helpers.cpp#308 */
    sqlite3_result_int64(context, ((hash(url, 0, i_colon - 1) & 0x0000FFFF) << 32) + hash(url, 0, len - 1));
}

#ifdef _WIN32
__declspec(dllexport)
#endif
int sqlite3_sqlitemozillaurlhash_init(sqlite3 *db, char **pzErrMsg, const sqlite3_api_routines *pApi){
    SQLITE_EXTENSION_INIT2(pApi);
    return sqlite3_create_function(db, "hash", 1, SQLITE_UTF8, 0, mozilla_url_hash, 0, 0);
}
