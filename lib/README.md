# lib/

Prebuilt **`librobotos_fw.a`** ships in this workshop clone (students do **not**
build or clone `firmware-esp32-s3-dev`). Instructors refresh it from a
`robotos-fw-*.tar.gz` release when needed.

```text
lib/librobotos_fw.a
include/firmware-esp32-s3-dev/   # sibling of lib/ — same release tarball
```

If `librobotos_fw.a` is missing, ask the instructor — demos will not link.
Do not commit a hand-built archive from an unclean tree.
