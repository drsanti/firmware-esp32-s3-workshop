/*
 * ex05-event-group — lab switcher
 *
 * WHAT THIS CODE DOES
 *   Selects which step of this special lab actually runs.
 *   main/main.c calls ex05_start(); uncomment exactly one step start
 *   below.
 *
 * HOW IT WORKS
 *   All step .c files are linked into the firmware.
 *   Only the one start() invoked here creates tasks / enters its loop.
 *
 * PROBLEM WITHOUT THIS PATTERN
 *   Separate IDF apps per step force constant cd/reconfigure.
 *
 * PROBLEM THIS FILE SOLVES
 *   One flashable lab folder; step choice stays next to the step sources.
 *
 * WHAT YOU LEARN HERE
 *   - Two-level enable: main.c picks the lab; this file picks the step
 *
 * Step labsheets: steps/<step>/README.md
 */
#include "ex05.h"

#include "ex05_step01.h"
#include "ex05_step02.h"
#include "ex05_step03.h"

void ex05_start(void)
{
    ex05_step01_start();  // step-01-checklist
    // ex05_step02_start();  // step-02-one-bit-arm
    // ex05_step03_start();  // step-03-wait-bits
}