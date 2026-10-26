/*
 * rb_version.h — firmware library version identity.
 *
 * Stamped at archive build time (see esp32s3/components/robotos_fw)
 * so a shipped librobotos_fw.a + headers pair reports what it was
 * built from. Override with -DROBOTOS_FW_VERSION=\"x.y.z\" when
 * packaging a release cut that isn't HEAD.
 */
#ifndef RB_VERSION_H
#define RB_VERSION_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ROBOTOS_FW_VERSION
#define ROBOTOS_FW_VERSION "0.3.0"
#endif

#define ROBOTOS_FW_VERSION_MAJOR 0
#define ROBOTOS_FW_VERSION_MINOR 3
#define ROBOTOS_FW_VERSION_PATCH 0

/* Human-readable version string, e.g. "robotos-fw/0.3.0". */
const char *rb_fw_version(void);

#ifdef __cplusplus
}
#endif

#endif /* RB_VERSION_H */
