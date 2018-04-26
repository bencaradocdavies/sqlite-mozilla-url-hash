#include <stdint.h>
#include <stdio.h>

static void test(char *url, uint64_t hash) {
    printf("\"%s\" %lu\n", url, hash);
}

int main(int argc, char *argv[]) {
    test("", 0UL);
    test(":", 0UL);
    test("http://example.org/", 125508604170377UL);
    test("https://example.org/", 47358175329495UL);
    test("https://www.reddit.com/", 47359719085711UL);
    test("https://old.reddit.com/", 47358033120677UL);
    test("https://en.wikipedia.org/wiki/Libert%C3%A9", 47359238090423UL);
}
