#include "ghost.h"
#include "../map/map.h"
#include "../player/player.h"
#include <stdlib.h>
#include <stddef.h>

/* ===================== */
/*   INTERNAL HELPERS    */
/* ===================== */

// All four cardinal directions as an array — used when picking a random turn
static const direction_t ALL_DIRS[4] = { DIR_UP, DIR_DOWN, DIR_LEFT, DIR_RIGHT };

// Returns the opposite of a direction — ghosts never reverse unless forced
static direction_t opposite_dir(direction_t dir) {
    switch (dir) {
        case DIR_UP:    return DIR_DOWN;
        case DIR_DOWN:  return DIR_UP;
        case DIR_LEFT:  return DIR_RIGHT;
        case DIR_RIGHT: return DIR_LEFT;
        default:        return DIR_NONE;
    }
}

// Converts a direction + speed into pixel deltas
static void dir_to_delta(direction_t dir, int speed, int *dx, int *dy) {
    *dx = 0;
    *dy = 0;
    switch (dir) {
        case DIR_UP:    *dy = -speed; break;
        case DIR_DOWN:  *dy =  speed; break;
        case DIR_LEFT:  *dx = -speed; break;
        case DIR_RIGHT: *dx =  speed; break;
        default: break;
    }
}

// Returns true if the ghost at (px, py) has all four bounding-box corners
// on walkable tiles. Mirrors the same logic used in player.c for consistency.
static bool can_move_to(const map_t *map, int px, int py) {
    int corners[4][2] = {
        { px + 1,             py + 1             },
        { px + TILE_SIZE - 2, py + 1             },
        { px + 1,             py + TILE_SIZE - 2 },
        { px + TILE_SIZE - 2, py + TILE_SIZE - 2 },
    };

    for (int i = 0; i < 4; i++) {
        int col, row;
        map_tile_from_pixel(corners[i][0], corners[i][1], &row, &col);
        if (!map_is_walkable(map, row, col))
            return false;
    }
    return true;
}

// Returns true if the ghost is aligned to the tile grid on the axis
// perpendicular to its direction of travel. Only at these moments is
// a direction change allowed — this keeps ghosts inside corridors.
static bool is_tile_aligned(const ghost_t *ghost) {
    int rel_x = (ghost->x - MAP_OFFSET_X) % TILE_SIZE;
    int rel_y = (ghost->y - MAP_OFFSET_Y) % TILE_SIZE;

    // Allow a small tolerance window equal to GHOST_SPEED so we never miss it
    bool aligned_x = (rel_x <= GHOST_SPEED || rel_x >= TILE_SIZE - GHOST_SPEED);
    bool aligned_y = (rel_y <= GHOST_SPEED || rel_y >= TILE_SIZE - GHOST_SPEED);

    switch (ghost->direction) {
        case DIR_LEFT:
        case DIR_RIGHT: return aligned_y;  // moving horizontally → must be aligned on Y
        case DIR_UP:
        case DIR_DOWN:  return aligned_x;  // moving vertically   → must be aligned on X
        default:        return true;        // DIR_NONE: always allow a pick
    }
}

// Core random movement logic used in GHOST_CHASE state.
// At each tile junction the ghost picks a random walkable direction,
// never reversing unless it is the only option (dead end).
static void ghost_move_random(ghost_t *ghost, const map_t *map) {
    int speed = GHOST_SPEED;
    int dx, dy;

    // If not yet at a junction boundary, just keep going straight
    if (!is_tile_aligned(ghost)) {
        if (ghost->direction == DIR_NONE) return;
        dir_to_delta(ghost->direction, speed, &dx, &dy);
        if (can_move_to(map, ghost->x + dx, ghost->y + dy)) {
            ghost->x += dx;
            ghost->y += dy;
        }
        return;
    }

    // At a junction: build a list of candidate directions
    // (exclude current reverse unless it is the only option)
    direction_t candidates[4];
    int n = 0;
    direction_t reverse = opposite_dir(ghost->direction);

    for (int i = 0; i < 4; i++) {
        direction_t d = ALL_DIRS[i];
        if (d == reverse) continue;          // skip reverse on first pass
        dir_to_delta(d, speed, &dx, &dy);
        if (can_move_to(map, ghost->x + dx, ghost->y + dy))
            candidates[n++] = d;
    }

    // If completely stuck (dead end), allow reversing
    if (n == 0 && reverse != DIR_NONE) {
        dir_to_delta(reverse, speed, &dx, &dy);
        if (can_move_to(map, ghost->x + dx, ghost->y + dy)) {
            candidates[n++] = reverse;
        }
    }

    if (n == 0) return;  // fully blocked — shouldn't happen in a valid map

    // Pick one of the candidates at random
    direction_t chosen = candidates[rand() % n];
    ghost->direction = chosen;

    dir_to_delta(chosen, speed, &dx, &dy);
    ghost->x += dx;
    ghost->y += dy;
}

// Frightened movement: same random logic but at half speed (every other tick).
// We reuse ghost_move_random but gated by the state_ticks_remaining parity.
static void ghost_move_frightened(ghost_t *ghost, const map_t *map) {
    // Move only on even ticks of the countdown → effectively half speed
    if (ghost->state_ticks_remaining % 2 == 0)
        ghost_move_random(ghost, map);
}

// Respawn: snap the ghost back to its spawn pixel position and resume CHASE
static void ghost_respawn(ghost_t *ghost) {
    ghost->x         = ghost->spawn_x;
    ghost->y         = ghost->spawn_y;
    ghost->direction = DIR_LEFT;    // pick an initial direction out of the pen
    ghost->state     = GHOST_CHASE;
    ghost->state_ticks_remaining = 0;
}

/* ===================== */
/*   LIFECYCLE           */
/* ===================== */

ghost_t *ghost_create(int x, int y, ghost_id_t id) {
    ghost_t *ghost = malloc(sizeof(ghost_t));
    if (!ghost) return NULL;

    ghost->x                    = x;
    ghost->y                    = y;
    ghost->spawn_x              = x;
    ghost->spawn_y              = y;
    ghost->direction            = DIR_LEFT;
    ghost->state                = GHOST_CHASE;
    ghost->id                   = id;
    ghost->state_ticks_remaining = 0;
    ghost->anim_frame           = 0;
    ghost->anim_tick_counter    = 0;

    return ghost;
}

void ghost_destroy(ghost_t *ghost) {
    free(ghost);
}

int ghosts_create_all(const map_t *map, ghost_t *ghosts[GHOST_COUNT]) {
    int count = 0;

    for (int row = 0; row < MAP_ROWS && count < GHOST_COUNT; row++) {
        for (int col = 0; col < MAP_COLS && count < GHOST_COUNT; col++) {
            if (map->tiles[row][col].type == TILE_GHOST_SPAWN) {
                int px = MAP_OFFSET_X + col * TILE_SIZE;
                int py = MAP_OFFSET_Y + row * TILE_SIZE;
                ghosts[count] = ghost_create(px, py, (ghost_id_t)count);
                if (!ghosts[count]) return count;  // partial success
                count++;
            }
        }
    }

    // Fill remaining slots with NULL if fewer spawn tiles exist than GHOST_COUNT
    for (int i = count; i < GHOST_COUNT; i++)
        ghosts[i] = NULL;

    return count;
}

void ghosts_destroy_all(ghost_t *ghosts[GHOST_COUNT], int count) {
    for (int i = 0; i < count; i++) {
        if (ghosts[i]) {
            ghost_destroy(ghosts[i]);
            ghosts[i] = NULL;
        }
    }
}

void ghosts_reset_all(ghost_t *ghosts[GHOST_COUNT], int count) {
    for (int i = 0; i < count; i++) {
        if (ghosts[i])
            ghost_respawn(ghosts[i]);
    }
}

/* ===================== */
/*   PER-TICK UPDATES    */
/* ===================== */

void ghost_tick(ghost_t *ghost, const map_t *map) {
    if (!ghost) return;

    // --- State countdown ---
    switch (ghost->state) {

        case GHOST_FRIGHTENED:
            ghost_move_frightened(ghost, map);
            if (ghost->state_ticks_remaining > 0)
                ghost->state_ticks_remaining--;
            if (ghost->state_ticks_remaining == 0)
                ghost->state = GHOST_CHASE;
            break;

        case GHOST_DEAD:
            if (ghost->state_ticks_remaining > 0)
                ghost->state_ticks_remaining--;
            if (ghost->state_ticks_remaining == 0)
                ghost_respawn(ghost);
            break;  // don't move while dead

        case GHOST_CHASE:
        default:
            ghost_move_random(ghost, map);
            break;
    }

    // --- Animation ---
    ghost->anim_tick_counter++;
    if (ghost->anim_tick_counter >= GHOST_ANIM_SPEED) {
        ghost->anim_tick_counter = 0;
        ghost->anim_frame = (ghost->anim_frame + 1) % GHOST_ANIM_FRAMES;
    }
}

void ghosts_tick_all(ghost_t *ghosts[GHOST_COUNT], int count, const map_t *map) {
    for (int i = 0; i < count; i++) {
        if (ghosts[i])
            ghost_tick(ghosts[i], map);
    }
}

/* ===================== */
/*   STATE CHANGES       */
/* ===================== */

void ghost_frighten(ghost_t *ghost) {
    if (!ghost) return;
    // Dead ghosts are not affected — they are already out of play
    if (ghost->state == GHOST_DEAD) return;

    ghost->state                = GHOST_FRIGHTENED;
    ghost->state_ticks_remaining = GHOST_FRIGHTENED_TICKS;

    // Reverse direction immediately — classic Pac-Man behaviour on power-up
    direction_t rev = opposite_dir(ghost->direction);
    if (rev != DIR_NONE)
        ghost->direction = rev;
}

void ghosts_frighten_all(ghost_t *ghosts[GHOST_COUNT], int count) {
    for (int i = 0; i < count; i++) {
        if (ghosts[i])
            ghost_frighten(ghosts[i]);
    }
}

void ghost_eat(ghost_t *ghost) {
    if (!ghost) return;
    ghost->state                = GHOST_DEAD;
    ghost->state_ticks_remaining = GHOST_DEAD_TICKS;
    ghost->direction            = DIR_NONE;
}

/* ===================== */
/*   COLLISION           */
/* ===================== */

bool ghost_collides_with_player(const ghost_t *ghost, const player_t *player) {
    if (!ghost || !player) return false;

    // Dead ghosts cannot interact with the player
    if (ghost->state == GHOST_DEAD) return false;

    // AABB overlap — same bounding box size (TILE_SIZE) for both entities
    return ghost->x < player->x + TILE_SIZE &&
           ghost->x + TILE_SIZE > player->x &&
           ghost->y < player->y + TILE_SIZE &&
           ghost->y + TILE_SIZE > player->y;
}

int ghosts_check_collision(ghost_t *ghosts[GHOST_COUNT], int count, const player_t *player) {
    for (int i = 0; i < count; i++) {
        if (ghosts[i] && ghost_collides_with_player(ghosts[i], player))
            return i;  // return index so caller knows which ghost was hit
    }
    return -1;  // no collision
}
