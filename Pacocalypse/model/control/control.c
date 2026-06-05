#include "control.h"

static void update_alive_time(game_state_t *state) {
    if (!state || state->mode != STATE_PLAYING)
        return;

    if (state->alive_tick_counter < TICKRATE)
        state->alive_tick_counter++;

    if (state->rtc_timer_started && state->alive_tick_counter < TICKRATE)
        return;

    rtc_time current_time;
    if (rtc_read_time(&current_time) != 0) {
        if (state->alive_tick_counter >= TICKRATE) {
            state->alive_seconds++;
            state->alive_tick_counter = 0;
        }
        return;
    }

    uint32_t current_seconds = rtc_time_to_seconds(&current_time);

    if (!state->rtc_timer_started) {
        state->rtc_start_seconds = current_seconds;
        state->alive_seconds = 0;
        state->alive_tick_counter = 0;
        state->rtc_timer_started = true;
        return;
    }

    if (current_seconds >= state->rtc_start_seconds) {
        state->alive_seconds = current_seconds - state->rtc_start_seconds;
    } else {
        state->alive_seconds = (24 * 60 * 60 - state->rtc_start_seconds) + current_seconds;
    }

    state->alive_tick_counter = 0;
}

bool update_keyboard_state(game_state_t *state) {
    if (!state) return false;

    kbc_ih();
    uint8_t scancode = 0;

    if (!get_scancode_status()) {
        return false;
    }

    set_scancode_status(false);
    scancode = get_scancode();

    switch (state->mode) {
        case STATE_PLAYING:
            switch (scancode) {
                case W_MAKE: player_set_direction(state->player, DIR_UP); break;
                case A_MAKE: player_set_direction(state->player, DIR_LEFT); break;
                case S_MAKE: player_set_direction(state->player, DIR_DOWN); break;
                case D_MAKE: player_set_direction(state->player, DIR_RIGHT); break;
                case ESC_MAKE: state->mode = STATE_PAUSED; break;
                default: break;
            }
            break;
        case STATE_PAUSED:
            if (scancode == ESC_MAKE)
                state->mode = STATE_PLAYING;
            break;
        case STATE_GAME_OVER:
            if (scancode == ESC_MAKE)
                state->mode = STATE_MENU;
            break;
        default:
            break;
    }

    return state->mode == STATE_QUIT;
}

bool update_mouse_state(game_state_t *state,
                        input_state_t *input,
                        menu_state_t *menu,
                        pause_state_t *pause) {
    if (!state) return false;
    if (!input) return false;

    mouse_ih();

    if (!mouse_get_byte_ready())
        return false;

    mouse_set_byte_ready(false);
    uint8_t byte = mouse_get_byte();

    struct packet pp;
    if (!mouse_parse_packet(byte, &pp))
        return false;

    input_update(input, &pp);

    switch (state->mode) {
        case STATE_PLAYING:
            game_handle_mouse_click(state, input);
            break;
        case STATE_MENU:
            menu_handle_mouse(menu, input, state);
            break;
        case STATE_PAUSED:
            pause_handle_mouse(pause, input, state);
            break;
        default:
            break;
    }

    input_reset_clicks(input);
    return state->mode == STATE_QUIT;
}

bool update_timer_state(game_state_t *state,
                        const view_resources_t *resources,
                        const input_state_t *input,
                        const menu_state_t *menu,
                        const pause_state_t *pause) {
    if (!state) return false;

    timer_ih();
    if (game_update(state) != 0) {
        printf("Error updating game state\n");
        return true;
    }

    update_alive_time(state);
    renderer_update_animations(state, resources);
    renderer_draw_game(state, resources, input, menu, pause);

    return state->mode == STATE_QUIT;
}
