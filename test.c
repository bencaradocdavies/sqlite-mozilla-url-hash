/*
 * Tests for hash.c.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>

uint64_t hash(const unsigned char *url, int len);

static int test(const char *url, uint64_t expected) {
    uint64_t actual = hash((const unsigned char *) url, strlen(url));
    printf("url=\"%s\", expected=%lu, actual=%lu, result=", url, expected, actual);
    if (expected == actual) {
        printf("PASS\n");
        return 0;
    } else {
        printf("***FAIL***\n");
        return 1;
    }
}

int main(int argc, char *argv[]) {
    int count = 0;
    count += test("http://example.org/", 125508604170377UL);
    count += test("https://example.org/", 47358175329495UL);
    count += test("https://www.reddit.com/", 47359719085711UL);
    count += test("https://old.reddit.com/", 47358033120677UL);
    count += test("https://en.wikipedia.org/wiki/Libert%C3%A9", 47359238090423UL);
    if (count == 0) {
        printf("All tests PASSED\n");
    } else {
        printf("Tests FAILED: %d\n", count);
    }
    return count;
}
