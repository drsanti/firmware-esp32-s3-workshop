/*
 * ex01-mutex-beep — lab switcher
 *
 * WHAT THIS CODE DOES
 *   Selects which step of this special lab actually runs.
 *   main/main.c calls ex01_start(); uncomment exactly one step start
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
#include "ex01.h"

#include "ex01_step01.h"
#include "ex01_step02.h"
#include "ex01_step03.h"

void ex01_start(void)
{
    ex01_step01_start();  // step-01-two-voices
    // ex01_step02_start();  // step-02-overlap-crash
    // ex01_step03_start();  // step-03-mutex-guard
}