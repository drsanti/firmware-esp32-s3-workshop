/*
 * ex06-task-monitor — lab switcher
 *
 * WHAT THIS CODE DOES
 *   Selects which step of this device lab actually runs.
 *   main/main.c calls ex06_start(); uncomment exactly one step start
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
#include "ex06.h"

#include "ex06_step01.h"
#include "ex06_step02.h"
#include "ex06_step03.h"
#include "ex06_step04.h"
#include "ex06_step05.h"

void ex06_start(void)
{
    // ex06_step01_start();  // step-01-name-face
    // ex06_step02_start();  // step-02-live-names
    // ex06_step03_start();  // step-03-state-prio
    // ex06_step04_start();  // step-04-stack-hwm
    ex06_step05_start();  // step-05-queue-depth
}