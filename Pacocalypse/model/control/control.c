#include "control.h"

void update_keyboard_state(player_t *player) {
    kbc_ih();
    switch (kbc_get_scancode()) {
        case W_KEY:
            player->next_direction = DIR_UP;
            break;
        case A_KEY:
            player->next_direction = DIR_LEFT;
            break;
        case S_KEY:
            player->next_direction = DIR_DOWN;
            break;
        case D_KEY:
            player->next_direction = DIR_RIGHT;
            break;
        default:
            break;
    }
}

void update_mouse_state(game_state_t *state) {
    static struct packet pp;
    mouse_ih();
    
    if (mouse_parse_packet(mouse_get_byte(), &pp)) {
        // Complete packet received
        game_handle_mouse(state, &pp);
    }
}

void update_timer_state(game_state_t *state) {
    timer_ih();
    game_update(state);
    renderer_draw_game(state);
    vg_flip();  // Hardware buffer swap
}

void update_rtc_state() {
    static int tick_count = 0;
    
    tick_count++;
    
    // Only update RTC every TICKRATE ticks (1 second at 60 Hz)
    if (tick_count >= TICKRATE) {
        rtc_date date;
        if (rtc_read_date(&date) == 0) {
            // RTC updated successfully
            // TODO: Store date in game state if needed for display
        }
        tick_count = 0;
    }
}
