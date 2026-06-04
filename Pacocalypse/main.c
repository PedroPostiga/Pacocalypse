#include <lcom/lcf.h>
#include <stdbool.h>
#include <stdint.h>

#include "../lab2/timer.h"
#include "../lab3/kbc.h"
#include "../lab4/mouse.h"
#include "../lab5/videocard.h"
#include "model/game/game.h"
#include "model/player/player.h"
#include "model/ghost/ghost.h"
#include "view/renderer/renderer.h"
#include "view/sprite.h"

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
    
    game_state_t game_state;
    if (game_init(&game_state) != 0) {
        printf("Failed to initialize game state\n");
        return 1;
    }

    if (load_sprites(&game_state.sprites) != 0) {
        printf("Failed to load sprites\n");
        game_cleanup(&game_state);
        return 1;
    }

    renderer_draw_game(&game_state); // initial render

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
                    if (msg.m_notify.interrupts & BIT(timer_bit_no)) {
                        timer_ih();
                        if (game_update(&game_state) != 0) {
                            printf("Error updating game state\n");
                            done = true;
                        }
                        if (game_state.mode == STATE_QUIT) {
                            done = true;
                        }
                    }
                    if (msg.m_notify.interrupts & BIT(kbd_bit_no)) {
                        kbc_ih(); // error handling keyboard interrupt

                        if (get_scancode_status()) {
                            set_scancode_status(false);

                            uint8_t scancode = get_scancode();

                            switch (game_state.mode) {
                            case STATE_PLAYING:
                                switch (scancode) {
                                    case W_MAKE: player_set_direction(game_state.player, DIR_UP); break;
                                    case A_MAKE: player_set_direction(game_state.player, DIR_LEFT); break;
                                    case S_MAKE: player_set_direction(game_state.player, DIR_DOWN); break;
                                    case D_MAKE: player_set_direction(game_state.player, DIR_RIGHT); break;
                                    case ESC_MAKE: game_state.mode = STATE_PAUSED; break;
                                    default: break;
                                }
                                break;
                            case STATE_PAUSED:
                                if (scancode == ESC_MAKE) {
                                    game_state.mode = STATE_PLAYING;
                                }
                                break;
                            default:
                                break;
                            }
                        }
                    }
                    if (msg.m_notify.interrupts & BIT(mouse_bit_no)) {
                        mouse_ih(); // error handling mouse interrupt

                        struct packet pp;
                        
                        if (mouse_get_byte_ready()) {
                            mouse_set_byte_ready(false);
                            uint8_t byte = mouse_get_byte();

                            if (mouse_parse_packet(byte, &pp)) {
                                // handle mouse packet (e.g., for menu navigation)
                                
                            }
                        }
                        
                    }
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
    destroy_sprites(&game_state.sprites);
    if (renderer_cleanup() != 0) return 1;
    if (vg_exit() != 0) return 1;

    return 0;
}
