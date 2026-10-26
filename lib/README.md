# lib/

Place **`librobotos_fw.a`** here (from a `firmware-esp32-s3-dev` release).

```text
lib/librobotos_fw.a
include/firmware-esp32-s3-dev/   # sibling of lib/ — from the same tarball
```

Until a release is installed, demos will not link. Do not commit a
hand-built archive from an unclean tree.
