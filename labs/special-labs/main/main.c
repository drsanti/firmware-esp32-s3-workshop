/*
 * special-labs — exercise entry (lab switcher)
 *
 * WHAT THIS CODE DOES
 *   Boots the board once (bsp_init) and starts exactly one special lab.
 *   That lab's exNN.c then picks which step runs.
 *
 * HOW IT WORKS
 *   Include one exNN.h and call one exNN_start(); comment the others.
 *   Inside the lab folder, exNN.c chooses exNN_stepMM_start().
 *   app_main -> bsp_init -> exNN_start -> exNN_stepMM_start
 *
 * PROBLEM WITHOUT THIS PATTERN
 *   Many tiny IDF projects (one per step) make flashing slow and hide
 *   structure.
 *
 * PROBLEM THIS FILE SOLVES
 *   Single app binary; students switch labs here and steps in exNN.c.
 *
 * WHAT YOU LEARN HERE
 *   - One project / many exercises (same idea as device-labs / rtos-labs)
 *   - bsp_init belongs in app_main; steps must not re-init the board
 *
 * Index: ../README.md  (labs README specials section)
 */
#include "bsp.h"

#include "ex01.h"
/* #include "ex02.h" */
/* #include "ex03.h" */
/* #include "ex04.h" */
/* #include "ex05.h" */
/* #include "ex06.h" */

void app_main(void)
{
    bsp_init();
    ex01_start();
    /* ex02_start(); */
    /* ex03_start(); */
    /* ex04_start(); */
    /* ex05_start(); */
    /* ex06_start(); */
}