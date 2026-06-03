#include "game.h"

int game_init(game_state_t* state) {
    state->mode = STATE_PLAYING;  // CHANGE THIS TO MENU AFTER IMPLEMENTING IT
    state->map = map_create();
    state->player = player_create(state->map);
    state->num_ghosts = ghosts_create_all(state->map, state->ghosts);
    state->mouse_x = 512;  // Center of 1024x768 screen
    state->mouse_y = 384;
    
    if (state->player == NULL || state->map == NULL) {
        return 1; // error handling
    }
    
    if (state->num_ghosts == 0) {
        return 1; // must have at least one ghost
    }
    
    return 0; // success
}

void game_cleanup(game_state_t* state) {
    player_destroy(state->player);
    map_destroy(state->map);
    for (int i = 0; i < state->num_ghosts; i++) {
        ghost_destroy(state->ghosts[i]);
    }
}

void game_handle_mouse(game_state_t* state, struct packet* mouse_packet) {
    if (!state || !mouse_packet) return;
    
    // Extract signed X and Y movements from packet
    int8_t dx = (int8_t) mouse_packet->bytes[1];
    int8_t dy = (int8_t) mouse_packet->bytes[2];
    
    // Handle X overflow
    if (mouse_packet->bytes[0] & BIT(6)) {  // MOUSE_X_OVF
        dx = (dx > 0) ? 127 : -128;
    }
    
    // Handle Y overflow (inverted because mouse y increases downward)
    if (mouse_packet->bytes[0] & BIT(7)) {  // MOUSE_Y_OVF
        dy = (dy > 0) ? 127 : -128;
    }
    
    // Update mouse position
    state->mouse_x += dx;
    state->mouse_y -= dy;  // Invert Y for screen coordinates
    
    if (state->mouse_x < 0) state->mouse_x = 0;
    if (state->mouse_x > SCREEN_WIDTH - CURSOR_SIZE) state->mouse_x = SCREEN_WIDTH - CURSOR_SIZE;
    if (state->mouse_y < 0) state->mouse_y = 0;
    if (state->mouse_y > SCREEN_HEIGHT - CURSOR_SIZE) state->mouse_y = SCREEN_HEIGHT - CURSOR_SIZE;
}

int game_update(game_state_t* state) { // updates the state of the game, each one of the cases changes what updates should happen 60times/sec    
    switch (state->mode) {
        case STATE_MENU:
            break;
        case STATE_PLAYING: 
            break;
        case STATE_PAUSED:
            break;
        case STATE_GAME_OVER:
            break;
        case STATE_QUIT:
            break;
    }
    return 0;
}
