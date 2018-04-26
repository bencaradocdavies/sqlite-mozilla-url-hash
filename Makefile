all : test sqlite-mozilla-url-hash.so

test : test.c hash.c
	gcc -Wall -O2 $^ -o $@
	./$@

sqlite-mozilla-url-hash.so : hash.c
	gcc -Wall -O2 -fPIC -shared $< -o $@
	strip $@

clean :
	rm -f test sqlite-mozilla-url-hash.so 
