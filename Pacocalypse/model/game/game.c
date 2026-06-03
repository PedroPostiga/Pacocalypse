#include "game.h"
#include "../../view/sprite.h"

#define POWER_UP_CLICK_RADIUS 96

static void game_update_animations(void) {
    if (player_anim_up) animated_sprite_update(player_anim_up);
    if (player_anim_down) animated_sprite_update(player_anim_down);
    if (player_anim_left) animated_sprite_update(player_anim_left);
    if (player_anim_right) animated_sprite_update(player_anim_right);

    if (ghost_red) animated_sprite_update(ghost_red);
    if (ghost_pink) animated_sprite_update(ghost_pink);
    if (ghost_cyan) animated_sprite_update(ghost_cyan);
    if (ghost_orange) animated_sprite_update(ghost_orange);
    if (frightened_ghost_anim) animated_sprite_update(frightened_ghost_anim);
    if (ghost_eyes) animated_sprite_update(ghost_eyes);
}

static void game_check_collectibles(game_state_t* state) {
    if (!state || !state->player || !state->map) return;

    player_collect(state->player, state->map);

    if (map_all_pellets_collected(state->map)) {
        state->mode = STATE_GAME_OVER;
    }
}

static void game_check_ghost_collisions(game_state_t* state) {
    if (!state || !state->player) return;

    for (int i = 0; i < state->num_ghosts; i++) {
        ghost_t *ghost = state->ghosts[i];
        if (!ghost || !ghost_collides_with_player(ghost, state->player)) continue;

        if (ghost->state == GHOST_FRIGHTENED) {
            ghost_eat(ghost);
            player_add_score(state->player, SCORE_GHOST_EAT);
        }
        else if (ghost->state == GHOST_CHASE) {
            if (!player_die(state->player, state->map)) {
                state->mode = STATE_GAME_OVER;
                return;
            }
            ghosts_reset_all(state->ghosts, state->num_ghosts);
        }
    }
}

static void game_activate_power_up_at(game_state_t* state, int mouse_x, int mouse_y) {
    if (!state || !state->player || !state->player->powered_up) return;

    int radius_squared = POWER_UP_CLICK_RADIUS * POWER_UP_CLICK_RADIUS;

    for (int i = 0; i < state->num_ghosts; i++) {
        ghost_t *ghost = state->ghosts[i];
        if (!ghost || ghost->state == GHOST_DEAD || ghost->state == GHOST_RESPAWNING) continue;

        int ghost_center_x = ghost->x + TILE_SIZE / 2;
        int ghost_center_y = ghost->y + TILE_SIZE / 2;
        int dx = ghost_center_x - mouse_x;
        int dy = ghost_center_y - mouse_y;

        if (dx * dx + dy * dy <= radius_squared) {
            ghost_frighten(ghost);
        }
    }
}

int game_init(game_state_t* state) {
    if (!state) return 1;

    state->mode = STATE_PLAYING;  // CHANGE THIS TO MENU AFTER IMPLEMENTING IT
    state->map = map_create();
    if (state->map == NULL) {
        return 1;
    }

    state->player = player_create(state->map);
    if (state->player == NULL) {
        map_destroy(state->map);
        return 1;
    }

    state->num_ghosts = ghosts_create_all(state->map, state->ghosts);
    state->mouse_x = 512;  // Center of 1024x768 screen
    state->mouse_y = 384;

    if (state->num_ghosts == 0) {
        player_destroy(state->player);
        map_destroy(state->map);
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

    if (mouse_packet->bytes[0] & BIT(0)) {
        game_activate_power_up_at(state, state->mouse_x, state->mouse_y);
    }
}

int game_update(game_state_t* state) { // updates the state of the game, each one of the cases changes what updates should happen 60times/sec    
    if (!state) return 1;

    switch (state->mode) {
        case STATE_MENU:
            break;
        case STATE_PLAYING: 
            game_update_animations();
            player_tick_animation(state->player);
            player_tick_power_up(state->player);
            player_move(state->player, state->map);
            ghosts_update_mode(state->ghosts, state->num_ghosts);
            ghosts_tick_all_with_context(state->ghosts, state->num_ghosts, state->map, state->player);
            game_check_collectibles(state);
            game_check_ghost_collisions(state);
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
