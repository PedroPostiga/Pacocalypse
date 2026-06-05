#include "game.h"

#define POWER_UP_CLICK_RADIUS 96

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
        } else if (ghost->state == GHOST_ALIVE) {
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

    state->mode = STATE_MENU;
    state->map = map_create();
    if (!state->map) {
        return 1;
    }

    state->player = player_create(state->map);
    if (!state->player) {
        map_destroy(state->map);
        return 1;
    }

    state->num_ghosts = ghosts_create_all(state->map, state->ghosts);
    if (state->num_ghosts == 0) {
        player_destroy(state->player);
        map_destroy(state->map);
        return 1;
    }

    return 0;
}

void game_cleanup(game_state_t* state) {
    if (!state) return;

    if (state->player) {
        player_destroy(state->player);
        state->player = NULL;
    }

    if (state->map) {
        map_destroy(state->map);
        state->map = NULL;
    }

    for (int i = 0; i < state->num_ghosts; i++) {
        if (state->ghosts[i]) {
            ghost_destroy(state->ghosts[i]);
            state->ghosts[i] = NULL;
        }
    }

    state->num_ghosts = 0;
}

void game_handle_mouse_click(game_state_t* state, const input_state_t *input) {
    if (!state || !input || state->mode != STATE_PLAYING || !input->left_click) return;
    game_activate_power_up_at(state, input->mouse_x, input->mouse_y);
}

int game_update(game_state_t* state) {
    if (!state) return 1;

    switch (state->mode) {
        case STATE_MENU:
            break;
        case STATE_PLAYING:
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
