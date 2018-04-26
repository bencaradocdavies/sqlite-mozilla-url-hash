build:
	gcc -O2 -fPIC -shared sqlite3-mozilla-url-hash.c -o sqlite3-mozilla-url-hash.so
	strip sqlite3-mozilla-url-hash.so

clean:
	rm -f sqlite3-mozilla-url-hash.so
