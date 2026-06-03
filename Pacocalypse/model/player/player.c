#include "player.h"
#include "../map/map.h"
#include <stdlib.h>
#include <stddef.h>

/* ===================== */
/*   INTERNAL HELPERS    */
/* ===================== */

// Finds the pixel position of the TILE_PLAYER_SPAWN tile in the map.
// Sets (x, y) to the top-left corner of that tile.
// Returns true on success, false if no spawn tile is found.
static bool find_spawn(const map_t *map, int *out_x, int *out_y) {
    for (int row = 0; row < MAP_ROWS; row++) {
        for (int col = 0; col < MAP_COLS; col++) {
            if (map->tiles[row][col].type == TILE_PLAYER_SPAWN) {
                *out_x = MAP_OFFSET_X + col * TILE_SIZE;
                *out_y = MAP_OFFSET_Y + row * TILE_SIZE;
                return true;
            }
        }
    }
    return false;
}

// Returns the pixel delta (dx, dy) for one step in the given direction.
static void direction_to_delta(direction_t dir, int *dx, int *dy) {
    *dx = 0;
    *dy = 0;
    switch (dir) {
        case DIR_UP:    *dy = -PLAYER_SPEED; break;
        case DIR_DOWN:  *dy =  PLAYER_SPEED; break;
        case DIR_LEFT:  *dx = -PLAYER_SPEED; break;
        case DIR_RIGHT: *dx =  PLAYER_SPEED; break;
        default: break;
    }
}

// Returns true if the player at position (px, py) can move without entering a wall.
// Checks all four corners of the player sprite (TILE_SIZE x TILE_SIZE bounding box).
static bool can_move_to(const map_t *map, int px, int py) {
    // Four corners of the bounding box (shrink by 1 to avoid edge collisions on border)
    int corners[4][2] = {
        { px + 1,              py + 1              },  // top-left
        { px + TILE_SIZE - 2,  py + 1              },  // top-right
        { px + 1,              py + TILE_SIZE - 2  },  // bottom-left
        { px + TILE_SIZE - 2,  py + TILE_SIZE - 2  },  // bottom-right
    };

    for (int i = 0; i < 4; i++) {
        int col, row;
        map_tile_from_pixel(corners[i][0], corners[i][1], &row, &col);
        if (!map_is_walkable(map, row, col))
            return false;
    }
    return true;
}

/* ===================== */
/*   LIFECYCLE           */
/* ===================== */

player_t *player_create(const map_t *map) {
    player_t *player = malloc(sizeof(player_t));
    if (!player) return NULL;

    int spawn_x, spawn_y;
    if (!find_spawn(map, &spawn_x, &spawn_y)) {
        free(player);
        return NULL;
    }

    player->x                       = spawn_x;
    player->y                       = spawn_y;
    player->direction               = DIR_NONE;
    player->next_direction          = DIR_NONE;
    player->lives                   = PLAYER_LIVES;
    player->score                   = 0;
    player->powered_up              = false;
    player->power_up_ticks_remaining = 0;
    player->anim_frame              = 0;
    player->anim_tick_counter       = 0;
    player->alive                   = true;

    return player;
}

void player_destroy(player_t *player) {
    free(player);
}

/* ===================== */
/*   PER-TICK UPDATES    */
/* ===================== */

void player_move(player_t *player, map_t *map) {
    if (!player->alive) return;

    int dx, dy;

    // Try the buffered (next) direction first
    if (player->next_direction != DIR_NONE) {
        direction_to_delta(player->next_direction, &dx, &dy);
        int nx = player->x + dx;
        int ny = player->y + dy;

        if (can_move_to(map, nx, ny)) {
            player->direction      = player->next_direction;
            player->next_direction = DIR_NONE;
            player->x = nx;
            player->y = ny;
            return;
        }
    }

    // Fall back to current direction
    if (player->direction == DIR_NONE) return;

    direction_to_delta(player->direction, &dx, &dy);
    int nx = player->x + dx;
    int ny = player->y + dy;

    if (can_move_to(map, nx, ny)) {
        player->x = nx;
        player->y = ny;
    }
    // If blocked, player stays in place (wall stop)
}

void player_tick_animation(player_t *player) {
    if (!player->alive) return;

    player->anim_tick_counter++;
    if (player->anim_tick_counter >= PLAYER_ANIM_SPEED) {
        player->anim_tick_counter = 0;
        player->anim_frame = (player->anim_frame + 1) % PLAYER_ANIM_FRAMES;
    }
}

void player_tick_power_up(player_t *player) {
    if (!player->powered_up) return;

    if (player->power_up_ticks_remaining > 0)
        player->power_up_ticks_remaining--;

    if (player->power_up_ticks_remaining == 0)
        player->powered_up = false;
}

/* ===================== */
/*   INPUT               */
/* ===================== */

void player_set_direction(player_t *player, direction_t dir) {
    player->next_direction = dir;
}

/* ===================== */
/*   GAME EVENTS         */
/* ===================== */

tile_type_t player_collect(player_t *player, map_t *map) {
    int row, col;
    player_get_tile(player, &row, &col);

    tile_type_t type = map_get_tile(map, row, col);

    if (type == TILE_PELLET) {
        if (!map->tiles[row][col].collected) {
            map_collect_pellet(map, row, col);
            player_add_score(player, SCORE_PELLET);
            return TILE_PELLET;
        }
    }
    else if (type == TILE_POWER_UP) {
        if (!map->tiles[row][col].collected) {
            map_collect_pellet(map, row, col); // reuses the same collect logic
            player_add_score(player, SCORE_POWER_UP);
            player_activate_power_up(player);
            return TILE_POWER_UP;
        }
    }

    return TILE_EMPTY;
}

void player_activate_power_up(player_t *player) {
    player->powered_up               = true;
    player->power_up_ticks_remaining = PLAYER_POWER_UP_DURATION;
}

bool player_die(player_t *player, const map_t *map) {
    player->alive = false;

    if (player->lives > 0)
        player->lives--;

    // Reset position to spawn
    int spawn_x, spawn_y;
    if (find_spawn(map, &spawn_x, &spawn_y)) {
        player->x = spawn_x;
        player->y = spawn_y;
    }

    // Reset movement and power-up state
    player->direction               = DIR_NONE;
    player->next_direction          = DIR_NONE;
    player->powered_up              = false;
    player->power_up_ticks_remaining = 0;
    player->anim_frame              = 0;
    player->anim_tick_counter       = 0;
    player->alive                   = true;

    if (player->lives == 0)
        return false;  // Game over

    return true;  // Still has lives
}

/* ===================== */
/*   SCORE               */
/* ===================== */

void player_add_score(player_t *player, uint32_t points) {
    player->score += points;
}

/* ===================== */
/*   HELPERS             */
/* ===================== */

void player_get_tile(const player_t *player, int *row, int *col) {
    // Use the centre of the sprite to determine the tile
    int centre_x = player->x + TILE_SIZE / 2;
    int centre_y = player->y + TILE_SIZE / 2;

    map_tile_from_pixel(centre_x, centre_y, row, col);
}

bool player_collides_with(const player_t *player, int x, int y, int w, int h) {
    // Simple AABB (axis-aligned bounding box) overlap check
    return player->x < x + w  &&
           player->x + TILE_SIZE > x &&
           player->y < y + h  &&
           player->y + TILE_SIZE > y;
}
