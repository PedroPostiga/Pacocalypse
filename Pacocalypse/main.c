#include <lcom/lcf.h>
#include <stdbool.h>
#include <stdint.h>

#include "controller/videocard/videocard.h"
#include "controller/rtc/rtc.h"
#include "controller/timer/timer.h"
#include "controller/keyboard/kbc.h"
#include "controller/mouse/mouse.h"
#include "model/game/game.h"

int (main)(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/Pacocalypse/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/Pacocalypse/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int (proj_main_loop)(int argc, char *argv[]) {

    if (vg_init(0x105) == NULL)
        return 1;
    
    if (vge_set_mode(0x105) != 0)
        return 1;
        
    if (vg_draw_rectangle(100, 100, 200, 200, 0xF) != 0)
        return 1;
    
    int ipc_status, r;
    message msg;
    
    uint8_t bit_no;
    if (kbd_subscribe_int(&bit_no)) return 1;
    int irq_set = BIT(bit_no);

    uint8_t timer_bit_no;
    if (timer_subscribe_int(&timer_bit_no)) return 1;
    int timer_irq_set = BIT(timer_bit_no);

    uint8_t bytes[2];
    int size = 0;
    bool two_byte = false;
    bool done = false;

    game_state_t state;
    state.mode = STATE_PLAYING;
    state.player = NULL;
    state.map = NULL;

    while (!done)
    {
        if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
            printf("driver_receive failed: %d\n", r);
            continue;
        }

        if (is_ipc_notify(ipc_status)) {
            switch (_ENDPOINT_P(msg.m_source)) {

                case HARDWARE:

                if (msg.m_notify.interrupts & timer_irq_set) {
                    timer_int_handler();
                    if (get_counter() % (60 / TICKRATE) == 0) {
                        if (game_update(&state) != 0) done = true;
                        if (game_render(&state) != 0) done = true;
                    }
                }

                if (msg.m_notify.interrupts & irq_set) {

                    kbc_read_scancode();

                    if (get_scancode_status()) {

                        set_scancode_status(false);

                        uint8_t scancode = get_scancode();

                        if (scancode == 0xE0) {
                            two_byte = true;
                            bytes[0] = scancode;
                            size = 1;
                        }
                        else {
                            if (two_byte) {
                            bytes[1] = scancode;
                            size = 2;
                            two_byte = false;
                            }
                            else {
                            bytes[0] = scancode;
                            size = 1;
                            }

                            if (scancode == ESC_BREAK_CODE) done = true;
                        }
                    }
                }
                break;

                default:
                break;
            }
        }
    }

    if (kbd_unsubscribe_int() != 0) return 1;

    if (vg_exit() != 0) return 1;

    if (timer_unsubscribe_int() != 0) return 1;

    return 0;
}