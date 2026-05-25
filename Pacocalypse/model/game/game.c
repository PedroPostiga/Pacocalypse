#include "game.h"

int game_init(game_state_t* state) {
    state->mode = STATE_PLAYING;  // CHANGE THIS TO MENU AFTER IMPLEMENTING IT
    state->map = map_create();
    state->player = player_create(state->map);
    state->num_ghosts = ghosts_create_all(state->map, state->ghosts);
    
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
    // Implementation for handling mouse events
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
