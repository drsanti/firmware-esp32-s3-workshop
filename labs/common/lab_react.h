/*
 * lab_react.h - Reaction Game status face (title + big state + detail).
 *
 *   react_init();
 *   react_start();
 *   react_set_state(REACT_ST_GO);
 *   react_set_detail("42 ms");
 */
#ifndef ROBOTOS_LAB_REACT_H
#define ROBOTOS_LAB_REACT_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    REACT_ST_READY = 0, /* press BOOT to start (later steps) */
    REACT_ST_WAIT,      /* random delay — do not press */
    REACT_ST_GO,        /* press BOOT now */
    REACT_ST_RESULT,    /* show time / false start */
} react_st_t;

void react_init(void);
void react_start(void);
void react_set_state(react_st_t st);
void react_set_detail(const char *fmt, ...);
void react_set_hint(const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif /* ROBOTOS_LAB_REACT_H */
