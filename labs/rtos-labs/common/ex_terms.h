/*
 * ex_terms — multi-pane rb_term grid for rtos-labs exercises.
 *
 * Hides display / screen / layout wiring so exNN.c stays on FreeRTOS.
 *
 *   ex_terms_begin(1, 2);
 *   rb_term_t *left = ex_terms_get(0);
 *   ex_terms_log(left, RB_TERM_CYAN, "hello %u", 1);
 */
#ifndef RTOS_LABS_EX_TERMS_H
#define RTOS_LABS_EX_TERMS_H

#include "rb_term.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef EX_TERMS_MAX
#define EX_TERMS_MAX 6
#endif

/* Open a rows x cols term grid (row-major pane indices). 0 = ok. */
int ex_terms_begin(unsigned rows, unsigned cols);

unsigned   ex_terms_count(void);
rb_term_t *ex_terms_get(unsigned index);

/* Colored log line (bg = black). No-op if t is NULL. */
void ex_terms_log(rb_term_t *t, uint16_t fg, const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif /* RTOS_LABS_EX_TERMS_H */
