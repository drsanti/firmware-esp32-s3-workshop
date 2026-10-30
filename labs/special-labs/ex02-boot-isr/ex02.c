/*
 * ex02-boot-isr — lab switcher
 *
 * WHAT THIS CODE DOES
 *   Selects which step of this special lab actually runs.
 *   main/main.c calls ex02_start(); uncomment exactly one step start
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
#include "ex02.h"

#include "ex02_step01.h"
#include "ex02_step02.h"
#include "ex02_step03.h"

void ex02_start(void)
{
    ex02_step01_start();  // step-01-poll-boot
    // ex02_step02_start();  // step-02-isr-busy
    // ex02_step03_start();  // step-03-isr-notify
}