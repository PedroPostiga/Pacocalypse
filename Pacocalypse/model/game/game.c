#include "game.h"

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

int game_render(game_state_t* state) { // draws different things depending on the mode
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
