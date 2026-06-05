#include <lcom/lcf.h>
#include <stdbool.h>
#include <stdint.h>

#include "../lab2/timer.h"
#include "../lab3/kbc.h"
#include "../lab4/mouse.h"
#include "../lab5/videocard.h"
#include "config.h"
#include "model/game/game.h"
#include "controller/control/control.h"
#include "controller/input/input.h"
#include "view/resources/resources.h"
#include "view/renderer/renderer.h"
#include "view/ui/menu.h"
#include "view/ui/pause.h"

int (main)(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  // lcf_trace_calls("/home/lcom/labs/Pacocalypse/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  // lcf_log_output("/home/lcom/labs/Pacocalypse/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int setup(uint8_t *timer_bit_no, uint8_t *kbd_bit_no, uint8_t *mouse_bit_no) {

    if (timer_set_frequency(0, TICKRATE) != 0)
        return 1;

    if (vg_init(VIDEO_MODE) == NULL)
        return 1;

    if (vge_set_mode(VIDEO_MODE) != 0)
        return 1;

    if (timer_subscribe_int(timer_bit_no) != 0)
        return 1;

    if (kbd_subscribe_int(kbd_bit_no) != 0)
        return 1;

    if (mouse_enable_data_reporting() != 0)
        return 1;
    
    if (mouse_subscribe_int(mouse_bit_no) != 0)
        return 1;

    if (renderer_init() != 0)
        return 1;

    return 0;
}

int (proj_main_loop)(int argc, char *argv[]) {

    uint8_t timer_bit_no, kbd_bit_no, mouse_bit_no;
    if (setup(&timer_bit_no, &kbd_bit_no, &mouse_bit_no) != 0)
        return 1;

    uint32_t timer_mask = BIT(timer_bit_no);
    uint32_t kbd_mask = BIT(kbd_bit_no);
    uint32_t mouse_mask = BIT(mouse_bit_no);

    game_state_t game_state;
    input_state_t input_state;
    menu_state_t menu_state;
    pause_state_t pause_state;
    view_resources_t view_resources;

    if (game_init(&game_state) != 0) {
        printf("Failed to initialize game state\n");
        return 1;
    }

    input_init(&input_state);
    input_state.mouse_x = SCREEN_WIDTH / 2;
    input_state.mouse_y = SCREEN_HEIGHT / 2;

    if (view_resources_init(&view_resources) != 0) {
        printf("Failed to initialize view resources\n");
        game_cleanup(&game_state);
        return 1;
    }

    menu_init(&menu_state, view_resources.game_font);
    pause_init(&pause_state, view_resources.game_font);

    renderer_draw_game(&game_state,
                       &view_resources,
                       input_state.mouse_x,
                       input_state.mouse_y,
                       &menu_state,
                       &pause_state); // initial render

    message msg;
    int ipc_status;
    bool done = false;

    while (!done) {
        if (driver_receive(ANY, &msg, &ipc_status) != 0) {
            printf("driver_receive failed\n");
            continue;
        }

        if (is_ipc_notify(ipc_status)) {
            switch (_ENDPOINT_P(msg.m_source)) {
                case HARDWARE:
                    if (msg.m_notify.interrupts & kbd_mask)
                        done = update_keyboard_state(&game_state) || done;

                    if (msg.m_notify.interrupts & mouse_mask)
                        done = update_mouse_state(&game_state,
                                                  &input_state,
                                                  &menu_state,
                                                  &pause_state) || done;

                    if (msg.m_notify.interrupts & timer_mask)
                        done = update_timer_state(&game_state,
                                                  &view_resources,
                                                  &input_state,
                                                  &menu_state,
                                                  &pause_state) || done;
                    break;
                default:
                    break;
            }
        }
    }
    if (mouse_unsubscribe_int() != 0) return 1;
    if (mouse_disable_data_reporting() != 0) return 1;
    if (kbd_unsubscribe_int() != 0) return 1;
    if (timer_unsubscribe_int() != 0) return 1;
    game_cleanup(&game_state);
    view_resources_cleanup(&view_resources);
    if (renderer_cleanup() != 0) return 1;
    if (vg_exit() != 0) return 1;

    return 0;
}
