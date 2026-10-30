/*
 * ex04-metronome — lab switcher
 *
 * WHAT THIS CODE DOES
 *   Selects which step of this device lab actually runs.
 *   main/main.c calls ex04_start(); uncomment exactly one step start
 *   below.
 *
 * HOW IT WORKS
 *   All step .c files are linked into the firmware.
 *   Only the one start() invoked here creates tasks / enters its loop.
 *
 * PROBLEM WITHOUT THIS PATTERN
 *   Separate IDF apps per step force constant cd/reconfigure.
 *   Putting every step in main.c hides the lab vs step structure.
 *
 * PROBLEM THIS FILE SOLVES
 *   One flashable lab folder; step choice stays next to the step sources.
 *
 * WHAT YOU LEARN HERE
 *   - Two-level enable: main.c picks the lab; this file picks the step
 *   - Comment/uncomment discipline (exactly one step active)
 *
 * Step labsheets: steps/<step>/README.md
 */
#include "ex04.h"

#include "ex04_step01.h"
#include "ex04_step02.h"
#include "ex04_step03.h"
#include "ex04_step04.h"
#include "ex04_step05.h"

void ex04_start(void)
{
    // ex04_step01_start();  // step-01-bpm-face
    // ex04_step02_start();  // step-02-ui-click
    // ex04_step03_start();  // step-03-feel-jitter
    // ex04_step04_start();  // step-04-tick-task
    ex04_step05_start();  // step-05-bpm-control
}