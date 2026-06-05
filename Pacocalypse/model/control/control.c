#include "control.h"

void update_keyboard_state(game_state_t *state) {
    if (!state) return;

    kbc_ih();

    if (!get_scancode_status())
        return;

    set_scancode_status(false);
    uint8_t scancode = get_scancode();

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
        default:
            break;
    }
}

bool update_mouse_state(input_state_t *input) {
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
    return true;
}

int update_timer_state(game_state_t *state) {
    if (!state) return 1;

    timer_ih();
    return game_update(state);
}
