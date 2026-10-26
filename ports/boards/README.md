# Board SKUs

Demos select a board with **`-DBOARD=<sku>`** (CMake cache). Always
**`set-target esp32s3`** first — both class boards are ESP32-S3.

| SKU | Hardware | Display | Touch |
|-----|----------|---------|-------|
| **`ws28p`** (default) | Waveshare ESP32-S3-Touch-LCD-2.8 (SKU 27690) | ST7789 | CST328 (V1) / CST3530 (V2) — auto-probed |
| **`es3c28p`** | Ternion ESP32-S3 reference | ILI9341 | FT6336 |

```bash
idf.py -DBOARD=ws28p  set-target esp32s3 build flash -p COMx
idf.py -DBOARD=es3c28p set-target esp32s3 build flash -p COMx
```

Switching SKU after a build: `idf.py fullclean` then rebuild with the
new `-DBOARD=…` (board sources are compiled into the demo, not the `.a`).
