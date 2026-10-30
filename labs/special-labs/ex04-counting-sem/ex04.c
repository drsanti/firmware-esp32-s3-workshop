/*
 * ex04-counting-sem — lab switcher
 *
 * WHAT THIS CODE DOES
 *   Selects which step of this special lab actually runs.
 *   main/main.c calls ex04_start(); uncomment exactly one step start
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
#include "ex04.h"

#include "ex04_step01.h"
#include "ex04_step02.h"
#include "ex04_step03.h"

void ex04_start(void)
{
    ex04_step01_start();  // step-01-three-slots
    // ex04_step02_start();  // step-02-oversubscribe
    // ex04_step03_start();  // step-03-sem-pool
}