# include/

Unpack the `include/firmware-esp32-s3-dev/` tree from a
`robotos-fw-*.tar.gz` release here so paths resolve as:

```text
include/firmware-esp32-s3-dev/rb.h
include/firmware-esp32-s3-dev/bsp.h
include/firmware-esp32-s3-dev/middleware/rb_*.h
…
```

Same release as `lib/librobotos_fw.a`. Prefer `#include "rb.h"` /
`"bsp.h"` / `"middleware/rb_mod.h"` (etc.) via the lab include path.
