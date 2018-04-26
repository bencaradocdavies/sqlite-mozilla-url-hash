# sqlite-mozilla-url-hash

SQLite extension to calculate the `url_hash` used in `places.sqlite` by Mozilla Firefox 50 and later.

## Overview

Mozilla Firefox stores bookmark URLs in the `moz_places` table of `places.sqlite` in the user profile directory. Mozilla Firefox 50 and later adds an `url_hash` column to the `moz_places` table, complicating the process of using SQLite tools for mass update of bookmark URLs. Such changes might be necessitated by, for example, a web server name change affecting many bookmarks. Manually updating hundred of bookmarks is tedious.

To add insult to injury, Mozilla Firefox 57 and later dropped support for legacy plugins and only supports WebExtensions. This change prevents the use of legacy plugins like [SQLite Manager](https://addons.mozilla.org/en-US/firefox/addon/sqlite-manager/) or [Places Maintenance ](https://addons.mozilla.org/en-US/firefox/addon/places-maintenance/) which had access to both the hash implementation and the places database and might have been able to rectify the situation. WebExtensions cannot access the places database.

It is still possible to use `sqlite3` or `sqlitebrowser` to modify the `url` column of `moz_places` in `places.sqlite`, but this leaves an `url_hash` value that does not match the `url`. Does such a mismatch have any unpleasant side effects? Will it cause Firefox to eat your bookmarks?

This SQLite extension adds a `hash` function that can be used to recalculate the `url_hash` column of `moz_places` in `places.sqlite` from the `url` column so that they match. It is a port of the Mozilla Firefox `url_hash` algorithm to C.

Tested with Firefox 59.0.2 on Debian unstable amd64.

## Dependencies

On Debian and similar, `sqlite3` is needed to use the extension, and `libsqlite3-dev` and some standard build tools are needed to build it:
```
apt-get install sqlite3 libsqlite3-dev libc6-dev make gcc binutils
```

## Build

Build the extension with:
```
make
```

The output library is `sqlite-mozilla-url-hash.so`.

## Precautions

Before you start:

1. Close Mozilla Firefox. Editing `places.sqlite` while Firefox is open may cause `places.sqlite` to be corrupted.

1. Create a backup of `places.sqlite` in case it is damaged by this procedure.

## Usage

Start `sqlite3` to edit `places.sqlite` (change the path if not in your current directory):
```
sqlite3 ./places.sqlite
```

You should now see the `sqlite3` prompt:
```
sqlite>
```

Load the extension (change the path if not in your current directory):
```
.load ./sqlite-mozilla-url-hash.so
```

Now the `hash` function should be available for use.

## Examples

All these examples are at the `sqlite3` prompt:
```
sqlite>
```

See if any places have a bad `url_hash` value:
```
select url, url_hash, hash(url) from moz_places where url_hash <> hash(url);
```

Don't like the Reddit redesign? Change all Reddit URLs to Old Reddit, setting `url_hash` to zero for simplicity, mass fixing in a later step:
```
update moz_places set url = replace(url, 'https://www.reddit.com/', 'https://old.reddit.com/'), url_hash = 0 where url like 'https://www.reddit.com/%';
```

See the mismatched `url_hash` values (note that `where url_hash = 0` should work just as well):
```
select url, url_hash, hash(url) from moz_places where url_hash <> hash(url);
```

Fix the mismatched `url_hash` values (note that `where url_hash = 0` should work just as well):
```
update moz_places set url_hash = hash(url) where url_hash <> hash(url);
```

There should now be no mismatched `url_hash` values:
```
select url, url_hash, hash(url) from moz_places where url_hash <> hash(url);
```

Database updates are immediate so you can exit with Ctrl-D or:
```
.exit
```

Now open Firefox and try your new bookmarks.

## References

The Mozilla Firefox hash implementation can be found in these files:
 * https://dxr.mozilla.org/mozilla-central/source/toolkit/components/places/Database.cpp
 * https://dxr.mozilla.org/mozilla-central/source/toolkit/components/places/Helpers.cpp
 * https://dxr.mozilla.org/mozilla-central/source/mfbt/HashFunctions.h

## Licence

This extension is licensed under the Mozilla Public License Version 2.0 because the implementation is based on code found within Mozilla Firefox.
