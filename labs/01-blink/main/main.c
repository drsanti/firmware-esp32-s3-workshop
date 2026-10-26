/*
 * Lab 01 — blink via librobotos_fw.
 * LED through board_led_set; node spin keeps the library alive.
 */
#include "board.h"
#include "rb.h"
#include "rb_serial.h"

static rb_node_t g_node;
static rb_serial_transport_t g_link;

static void delay_ms(uint32_t ms)
{
    uint64_t until = board_mono_ns() + (uint64_t)ms * 1000000ull;
    while (board_mono_ns() < until) {
        board_yield();
    }
}

void app_main(void)
{
    board_init();
    rb_transport_serial_init(&g_link);
    rb_node_init(&g_node, &(rb_node_cfg_t){
        .transport = &g_link.base,
        .name = "lab01_blink",
        .heartbeat_ms = 0,
    });

    for (;;) {
        board_led_set(1);
        rb_spin_once(&g_node);
        delay_ms(200);

        board_led_set(0);
        rb_spin_once(&g_node);
        delay_ms(200);
    }
}
