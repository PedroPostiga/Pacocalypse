#include "ghost.h"
#include "../map/map.h"
#include "../player/player.h"
#include <stdlib.h>
#include <stddef.h>
#include <limits.h>

/* ===================== */
/*   INTERNAL HELPERS    */
/* ===================== */

// All four cardinal directions as an array ??? used when picking a random turn
static const direction_t ALL_DIRS[4] = { DIR_UP, DIR_DOWN, DIR_LEFT, DIR_RIGHT };

// Corner targets for scatter mode (out of bounds so ghosts never fully reach them)
// These create the "patrolling corners" behavior of original Pac-Man
static const int CORNER_TARGETS[GHOST_COUNT][2] = {
    { MAP_OFFSET_X + MAP_COLS * TILE_SIZE + TILE_SIZE,  MAP_OFFSET_Y - TILE_SIZE },              // Red: top-right
    { MAP_OFFSET_X - TILE_SIZE,                          MAP_OFFSET_Y - TILE_SIZE },              // Pink: top-left
    { MAP_OFFSET_X + MAP_COLS * TILE_SIZE + TILE_SIZE,  MAP_OFFSET_Y + MAP_ROWS * TILE_SIZE },   // Cyan: bottom-right
    { MAP_OFFSET_X - TILE_SIZE,                          MAP_OFFSET_Y + MAP_ROWS * TILE_SIZE }    // Orange: bottom-left
};

typedef struct {
    ghost_mode_t mode;
    uint32_t ticks;
} ghost_mode_phase_t;

static const ghost_mode_phase_t MODE_PHASES[] = {
    { GHOST_MODE_SCATTER, 7 * 60 },
    { GHOST_MODE_CHASE, 20 * 60 },
    { GHOST_MODE_SCATTER, 7 * 60 },
    { GHOST_MODE_CHASE, 20 * 60 },
    { GHOST_MODE_SCATTER, 5 * 60 },
    { GHOST_MODE_CHASE, 20 * 60 },
    { GHOST_MODE_SCATTER, 5 * 60 },
    { GHOST_MODE_CHASE, 0 },
};

static int ghost_mode_phase = 0;

// Returns the opposite of a direction ??? ghosts never reverse unless forced
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


// Returns true if the entire ghost sprite (all 4 corners) is within the same single tile.
// This ensures ghosts can only change direction when fully centered in a tile.
static bool ghost_is_fully_in_single_tile(const ghost_t *ghost) {
    // Get the tile coordinates for each corner
    int corners[4][2] = {
        { ghost->x + 1,             ghost->y + 1             },              // Top-left
        { ghost->x + TILE_SIZE - 2, ghost->y + 1             },              // Top-right
        { ghost->x + 1,             ghost->y + TILE_SIZE - 2 },              // Bottom-left
        { ghost->x + TILE_SIZE - 2, ghost->y + TILE_SIZE - 2 }               // Bottom-right
    };

    // Calculate which tile the first corner is in
    int first_col = (corners[0][0] - MAP_OFFSET_X) / TILE_SIZE;
    int first_row = (corners[0][1] - MAP_OFFSET_Y) / TILE_SIZE;

    // Check that all corners are in the same tile
    for (int i = 0; i < 4; i++) {
        int col = (corners[i][0] - MAP_OFFSET_X) / TILE_SIZE;
        int row = (corners[i][1] - MAP_OFFSET_Y) / TILE_SIZE;

        if (col != first_col || row != first_row)
            return false;  // Corner is in a different tile
    }

    return true;  // All corners are in the same tile
}

// Calculates the Manhattan distance between two positions
static int shortest_distance(int x1, int y1, int x2, int y2) {
    return abs(x1 - x2) + abs(y1 - y2);
}

static direction_t pick_path_direction_to_tile(const ghost_t *ghost, const map_t *map, int target_row, int target_col) {
    int start_row, start_col;
    map_tile_from_pixel(ghost->x + TILE_SIZE / 2, ghost->y + TILE_SIZE / 2, &start_row, &start_col);

    if (start_row < 0 || start_row >= MAP_ROWS || start_col < 0 || start_col >= MAP_COLS)
        return DIR_NONE;
    if (target_row < 0 || target_row >= MAP_ROWS || target_col < 0 || target_col >= MAP_COLS)
        return DIR_NONE;
    if (!map_is_walkable(map, target_row, target_col))
        return DIR_NONE;

    int dist[MAP_ROWS][MAP_COLS];
    int queue_rows[MAP_ROWS * MAP_COLS];
    int queue_cols[MAP_ROWS * MAP_COLS];
    int head = 0, tail = 0;

    for (int row = 0; row < MAP_ROWS; row++) {
        for (int col = 0; col < MAP_COLS; col++) {
            dist[row][col] = -1;
        }
    }

    dist[target_row][target_col] = 0;
    queue_rows[tail] = target_row;
    queue_cols[tail] = target_col;
    tail++;

    while (head < tail) {
        int row = queue_rows[head];
        int col = queue_cols[head];
        head++;

        for (int i = 0; i < 4; i++) {
            int dx, dy;
            dir_to_delta(ALL_DIRS[i], 1, &dx, &dy);

            int next_row = row + dy;
            int next_col = col + dx;

            if (next_row < 0 || next_row >= MAP_ROWS || next_col < 0 || next_col >= MAP_COLS)
                continue;
            if (dist[next_row][next_col] != -1)
                continue;
            if (!map_is_walkable(map, next_row, next_col))
                continue;

            dist[next_row][next_col] = dist[row][col] + 1;
            queue_rows[tail] = next_row;
            queue_cols[tail] = next_col;
            tail++;
        }
    }

    direction_t best_dir = DIR_NONE;
    int best_distance = INT_MAX;

    for (int i = 0; i < 4; i++) {
        int dx, dy;
        dir_to_delta(ALL_DIRS[i], 1, &dx, &dy);

        int next_row = start_row + dy;
        int next_col = start_col + dx;

        if (next_row < 0 || next_row >= MAP_ROWS || next_col < 0 || next_col >= MAP_COLS)
            continue;
        if (dist[next_row][next_col] < 0 || dist[next_row][next_col] >= best_distance)
            continue;

        int move_dx, move_dy;
        dir_to_delta(ALL_DIRS[i], GHOST_SPEED, &move_dx, &move_dy);
        if (!can_move_to(map, ghost->x + move_dx, ghost->y + move_dy))
            continue;

        best_distance = dist[next_row][next_col];
        best_dir = ALL_DIRS[i];
    }

    return best_dir;
}

static direction_t pick_respawn_direction(const ghost_t *ghost, const map_t *map) {
    int spawn_row, spawn_col;
    map_tile_from_pixel(ghost->spawn_x + TILE_SIZE / 2, ghost->spawn_y + TILE_SIZE / 2, &spawn_row, &spawn_col);

    return pick_path_direction_to_tile(ghost, map, spawn_row, spawn_col);
}

// Picks the best direction towards a target by trying each cardinal direction
// and selecting the one that reduces distance to the target the most.
// Never reverses direction unless it's the only walkable option.
static direction_t pick_best_direction(const ghost_t *ghost, const map_t *map, int target_x, int target_y) {
    // Only allow direction changes when the ghost is fully within a single tile
    if (!ghost_is_fully_in_single_tile(ghost))
        return ghost->direction;

    direction_t best_dir = DIR_NONE;
    int best_distance = INT_MAX;
    int walkable_count = 0;
    direction_t reverse = opposite_dir(ghost->direction);

    // Try each direction
    for (int i = 0; i < 4; i++) {
        direction_t dir = ALL_DIRS[i];

        // Skip the reverse direction unless it's the only option
        if (dir == reverse)
            continue;

        // Check if we can move in this direction
        int dx, dy;
        dir_to_delta(dir, TILE_SIZE, &dx, &dy);
        int new_x = ghost->x + dx;
        int new_y = ghost->y + dy;

        if (!can_move_to(map, new_x, new_y))
            continue;

        walkable_count++;
        int distance = shortest_distance(new_x, new_y, target_x, target_y);

        if (distance < best_distance) {
            best_distance = distance;
            best_dir = dir;
        }
    }

    // If no valid direction found and reverse is our only option, use reverse
    if (best_dir == DIR_NONE && walkable_count == 0) {
        int dx, dy;
        dir_to_delta(reverse, TILE_SIZE, &dx, &dy);
        int new_x = ghost->x + dx;
        int new_y = ghost->y + dy;
        if (can_move_to(map, new_x, new_y))
            best_dir = reverse;
    }

    return (best_dir != DIR_NONE) ? best_dir : ghost->direction;
}

// Core movement logic for GHOST_CHASE state with AI targeting
// Each ghost has different targeting behavior based on mode:
// CHASE MODE:
// - Red: targets player's current position
// - Pink: targets 4 tiles ahead of player
// - Cyan (Blue): uses both player and red ghost position for vector calculation
// - Orange: chases player but retreats to corner when within 8 tiles
// SCATTER MODE:
// - All ghosts: target their designated corner (out of bounds)
static void ghost_move_normal(ghost_t *ghost, const map_t *map, const player_t *player, ghost_t *ghosts[GHOST_COUNT]) {
    if (!ghost || !player || !map) return;

    int speed = GHOST_SPEED;
    int dx, dy;
    int target_x = 0, target_y = 0;

    int row = -1, col = -1;
    map_tile_from_pixel(ghost->x + TILE_SIZE / 2, ghost->y + TILE_SIZE / 2, &row, &col);
    bool inside_house = (row >= 10 && row <= 11 && col >= 9 && col <= 11);

    // If ghost is inside the cage, override its target to be the exit of the house
    if (inside_house) {
        target_x = MAP_OFFSET_X + 10 * TILE_SIZE;
        target_y = MAP_OFFSET_Y + 9 * TILE_SIZE;
    }
    // Determine target position based on mode
    else if (ghost->mode == GHOST_MODE_SCATTER) {
        // Scatter mode: all ghosts target their corner (out of bounds)
        target_x = CORNER_TARGETS[ghost->id][0];
        target_y = CORNER_TARGETS[ghost->id][1];
    }
    else {
        // Chase mode: ghost-specific targeting
        if (ghost->id == GHOST_RED) {
            // Red ghost: target player's current position
            target_x = player->x;
            target_y = player->y;
        }
        else if (ghost->id == GHOST_PINK) {
            // Pink ghost: target 4 tiles ahead of player's current direction
            target_x = player->x;
            target_y = player->y;

            // Project 4 tiles (4 * TILE_SIZE pixels) ahead in player's direction
            int tiles_ahead = 4 * TILE_SIZE;
            switch (player->direction) {
                case DIR_UP:    target_y -= tiles_ahead; break;
                case DIR_DOWN:  target_y += tiles_ahead; break;
                case DIR_LEFT:  target_x -= tiles_ahead; break;
                case DIR_RIGHT: target_x += tiles_ahead; break;
                default: break;
            }
        }
        else if (ghost->id == GHOST_CYAN) {
            // Cyan ghost: complex targeting using red ghost position
            // 1. Find point 2 tiles ahead of player (not 4 for cyan)
            target_x = player->x;
            target_y = player->y;
            int tiles_ahead = 2 * TILE_SIZE;
            switch (player->direction) {
                case DIR_UP:    target_y -= tiles_ahead; break;
                case DIR_DOWN:  target_y += tiles_ahead; break;
                case DIR_LEFT:  target_x -= tiles_ahead; break;
                case DIR_RIGHT: target_x += tiles_ahead; break;
                default: break;
            }

            // 2. Find red ghost and compute vector from red to target point
            if (ghosts[GHOST_RED] != NULL) {
                int red_x = ghosts[GHOST_RED]->x;
                int red_y = ghosts[GHOST_RED]->y;

                // Vector from red ghost to target point
                int vector_x = target_x - red_x;
                int vector_y = target_y - red_y;

                // 3. Double the vector to get final target
                target_x = red_x + (vector_x * 2);
                target_y = red_y + (vector_y * 2);
            }
        }
        else if (ghost->id == GHOST_ORANGE) {
            // Orange ghost: chase player, but retreat if too close (within 8 tiles)
            int distance = shortest_distance(ghost->x, ghost->y, player->x, player->y);

            // 8 tiles = 8 * TILE_SIZE = 256 pixels
            if (distance < (8 * TILE_SIZE)) {
                // Too close: retreat to corner (bottom-left for orange)
                target_x = CORNER_TARGETS[GHOST_ORANGE][0];
                target_y = CORNER_TARGETS[GHOST_ORANGE][1];
            } else {
                // Chase player
                target_x = player->x;
                target_y = player->y;
            }
        }
    }

    // Pick best direction towards target and move
    direction_t best_dir = inside_house
        ? pick_path_direction_to_tile(ghost, map, 9, 10)
        : pick_best_direction(ghost, map, target_x, target_y);

    if (best_dir == DIR_NONE) {
        best_dir = pick_best_direction(ghost, map, target_x, target_y);
    }

    if (best_dir != ghost->direction && ghost_is_fully_in_single_tile(ghost)) {
        ghost->direction = best_dir;
    }

    // Move in current direction
    dir_to_delta(ghost->direction, speed, &dx, &dy);
    int new_x = ghost->x + dx;
    int new_y = ghost->y + dy;

    if (can_move_to(map, new_x, new_y)) {
        ghost->x = new_x;
        ghost->y = new_y;
    }
}

// Random movement: picks a random walkable direction at tile junctions
// Never reverses direction unless it's the only option (dead end)
static void ghost_move_random(ghost_t *ghost, const map_t *map) {
    if (!ghost || !map) return;

    int speed = GHOST_SPEED;
    int dx, dy;

    // Only pick direction when fully within a single tile
    if (!ghost_is_fully_in_single_tile(ghost)) {
        dir_to_delta(ghost->direction, speed, &dx, &dy);
        int new_x = ghost->x + dx;
        int new_y = ghost->y + dy;

        if (can_move_to(map, new_x, new_y)) {
            ghost->x = new_x;
            ghost->y = new_y;
        }
        return;
    }

    // Collect all walkable directions, preferring non-reverse
    direction_t walkable_dirs[4];
    int walkable_count = 0;
    direction_t reverse = opposite_dir(ghost->direction);

    for (int i = 0; i < 4; i++) {
        direction_t dir = ALL_DIRS[i];

        // Skip reverse direction unless it's the only option
        if (dir == reverse)
            continue;

        dir_to_delta(dir, TILE_SIZE, &dx, &dy);
        int new_x = ghost->x + dx;
        int new_y = ghost->y + dy;

        if (can_move_to(map, new_x, new_y)) {
            walkable_dirs[walkable_count++] = dir;
        }
    }

    // If no valid direction found (dead end), allow reverse
    if (walkable_count == 0) {
        dir_to_delta(reverse, TILE_SIZE, &dx, &dy);
        int new_x = ghost->x + dx;
        int new_y = ghost->y + dy;
        if (can_move_to(map, new_x, new_y)) {
            walkable_dirs[walkable_count++] = reverse;
        }
    }

    // Pick a random direction from walkable options
    if (walkable_count > 0) {
        int random_idx = rand() % walkable_count;
        ghost->direction = walkable_dirs[random_idx];
    }

    // Move in current direction
    dir_to_delta(ghost->direction, speed, &dx, &dy);
    int new_x = ghost->x + dx;
    int new_y = ghost->y + dy;

    if (can_move_to(map, new_x, new_y)) {
        ghost->x = new_x;
        ghost->y = new_y;
    }
}

// Frightened movement: random movement at half speed (every other tick)
static void ghost_move_frightened(ghost_t *ghost, const map_t *map) {
    // Move only on even ticks of the countdown ??? effectively half speed
    if (ghost->state_ticks_remaining % 2 == 0)
        ghost_move_random(ghost, map);
}

// Respawn movement: eyes head back to spawn through walkable corridors.
static void ghost_move_respawning(ghost_t *ghost, const map_t *map) {
    if (!ghost || !map) return;

    int speed = GHOST_SPEED;
    int dist_to_spawn = shortest_distance(ghost->x, ghost->y, ghost->spawn_x, ghost->spawn_y);
    if (dist_to_spawn <= speed) {
        ghost->x = ghost->spawn_x;
        ghost->y = ghost->spawn_y;
        ghost->direction = DIR_LEFT;
        ghost->state = GHOST_ALIVE;
        ghost->state_ticks_remaining = 0;
        return;
    }

    direction_t best_dir = pick_respawn_direction(ghost, map);
    if (best_dir == DIR_NONE) {
        best_dir = pick_best_direction(ghost, map, ghost->spawn_x, ghost->spawn_y);
    }

    if (best_dir != DIR_NONE && ghost_is_fully_in_single_tile(ghost)) {
        ghost->direction = best_dir;
    }

    int dx, dy;
    dir_to_delta(ghost->direction, speed, &dx, &dy);
    int new_x = ghost->x + dx;
    int new_y = ghost->y + dy;

    if (can_move_to(map, new_x, new_y)) {
        ghost->x = new_x;
        ghost->y = new_y;
        return;
    }

    direction_t reverse = opposite_dir(ghost->direction);
    for (int i = 0; i < 4; i++) {
        direction_t dir = ALL_DIRS[i];
        if (dir == reverse)
            continue;

        dir_to_delta(dir, speed, &dx, &dy);
        new_x = ghost->x + dx;
        new_y = ghost->y + dy;

        if (can_move_to(map, new_x, new_y)) {
            ghost->direction = dir;
            ghost->x = new_x;
            ghost->y = new_y;
            return;
        }
    }

    dir_to_delta(reverse, speed, &dx, &dy);
    new_x = ghost->x + dx;
    new_y = ghost->y + dy;

    if (can_move_to(map, new_x, new_y)) {
        ghost->direction = reverse;
        ghost->x = new_x;
        ghost->y = new_y;
    }
}

// Respawn: transition from DEAD to RESPAWNING state
static void ghost_respawn(ghost_t *ghost) {
    if (!ghost) return;

    // Transition to respawning state
    ghost->state = GHOST_RESPAWNING;
    ghost->state_ticks_remaining = 0;
}

static void ghost_reset_to_spawn(ghost_t *ghost) {
    if (!ghost) return;

    ghost->x = ghost->spawn_x;
    ghost->y = ghost->spawn_y;
    ghost->direction = DIR_LEFT;
    ghost->state = GHOST_ALIVE;
    ghost->state_ticks_remaining = 0;
    ghost->mode = MODE_PHASES[0].mode;
    ghost->mode_ticks_remaining = MODE_PHASES[0].ticks;
    ghost->anim_frame = 0;
    ghost->anim_tick_counter = 0;
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
    ghost->state                = GHOST_ALIVE;
    ghost->mode                 = MODE_PHASES[0].mode;
    ghost->id                   = id;
    ghost->state_ticks_remaining = 0;
    ghost->mode_ticks_remaining  = MODE_PHASES[0].ticks;
    ghost->anim_frame           = 0;
    ghost->anim_tick_counter    = 0;

    return ghost;
}

void ghost_destroy(ghost_t *ghost) {
    free(ghost);
}

int ghosts_create_all(const map_t *map, ghost_t *ghosts[GHOST_COUNT]) {
    int count = 0;
    ghost_mode_phase = 0;

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
    ghost_mode_phase = 0;

    for (int i = 0; i < count; i++) {
        if (ghosts[i])
            ghost_reset_to_spawn(ghosts[i]);
    }
}

/* ===================== */
/*   PER-TICK UPDATES    */
/* ===================== */

// Internal tick function with full context (used by ghosts_tick_all_with_context)
static void ghost_tick_internal(ghost_t *ghost, const map_t *map,
                                const player_t *player, ghost_t *ghosts[GHOST_COUNT]) {
    if (!ghost) return;

    // --- State countdown ---
    switch (ghost->state) {

        case GHOST_FRIGHTENED:
            ghost_move_frightened(ghost, map);
            if (ghost->state_ticks_remaining > 0)
                ghost->state_ticks_remaining--;
                if (ghost->state_ticks_remaining == 0)
                ghost->state = GHOST_ALIVE;
            break;

        case GHOST_DEAD:
            if (ghost->state_ticks_remaining > 0)
                ghost->state_ticks_remaining--;
            if (ghost->state_ticks_remaining == 0)
                ghost_respawn(ghost);
            break;  // don't move while dead

        case GHOST_RESPAWNING:
            ghost_move_respawning(ghost, map);
            // ghost_move_respawning handles state transition to CHASE when it reaches spawn
            break;

        case GHOST_ALIVE:
        default:
            ghost_move_normal(ghost, map, player, ghosts);
            break;
    }

    // --- Animation ---
    ghost->anim_tick_counter++;
    if (ghost->anim_tick_counter >= GHOST_ANIM_SPEED) {
        ghost->anim_tick_counter = 0;
        ghost->anim_frame = (ghost->anim_frame + 1) % GHOST_ANIM_FRAMES;
    }
}

void ghost_tick(ghost_t *ghost, const map_t *map) {
    // Legacy function - just handles animation without movement
    // For full movement with AI targeting, use ghosts_tick_all_with_context
    if (!ghost) return;

    // --- Animation only ---
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

// New function that updates all ghosts with full context (player and other ghosts)
// Call this from game_update instead of ghosts_tick_all
void ghosts_tick_all_with_context(ghost_t *ghosts[GHOST_COUNT], int count, const map_t *map,
                                   const player_t *player) {
    for (int i = 0; i < count; i++) {
        if (ghosts[i])
            ghost_tick_internal(ghosts[i], map, player, ghosts);
    }
}

/* ===================== */
/*   STATE CHANGES       */
/* ===================== */

void ghost_frighten(ghost_t *ghost) {
    if (!ghost) return;
    // Dead ghosts are not affected ??? they are already out of play
    if (ghost->state == GHOST_DEAD) return;

    ghost->state                = GHOST_FRIGHTENED;
    ghost->state_ticks_remaining = GHOST_FRIGHTENED_TICKS;

    // Reverse direction immediately ??? classic Pac-Man behaviour on power-up
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
    ghost_respawn(ghost);
    ghost->direction = opposite_dir(ghost->direction);
    if (ghost->direction == DIR_NONE)
        ghost->direction = DIR_LEFT;
}

/* ===================== */
/*   MODE MANAGEMENT     */
/* ===================== */

// Updates mode countdown and switches modes when timer expires
// Returns the current mode
ghost_mode_t ghosts_update_mode(ghost_t *ghosts[GHOST_COUNT], int count) {
    if (count == 0 || ghosts[0] == NULL)
        return GHOST_MODE_CHASE;  // Default if no ghosts

    // All ghosts share the same mode, so just check the first one
    ghost_t *ref_ghost = ghosts[0];
    int last_phase = (int)(sizeof(MODE_PHASES) / sizeof(MODE_PHASES[0])) - 1;

    // Decrement timer
    if (ref_ghost->mode_ticks_remaining > 0)
        ref_ghost->mode_ticks_remaining--;

    // Switch mode when timer expires. The final chase phase has 0 ticks and lasts forever.
    if (ref_ghost->mode_ticks_remaining == 0 && ghost_mode_phase < last_phase) {
        ghost_mode_phase++;

        for (int i = 0; i < count; i++) {
            if (ghosts[i]) {
                ghosts[i]->mode = MODE_PHASES[ghost_mode_phase].mode;
                ghosts[i]->mode_ticks_remaining = MODE_PHASES[ghost_mode_phase].ticks;
            }
        }
    }

    return ref_ghost->mode;
}

// Sets all ghosts to a specific mode immediately
void ghosts_set_mode(ghost_t *ghosts[GHOST_COUNT], int count, ghost_mode_t mode) {
    ghost_mode_phase = (mode == GHOST_MODE_SCATTER) ? 0 : 1;

    for (int i = 0; i < count; i++) {
        if (ghosts[i]) {
            ghosts[i]->mode = mode;
            ghosts[i]->mode_ticks_remaining = (mode == GHOST_MODE_CHASE) ?
                GHOST_CHASE_MODE_TICKS : GHOST_SCATTER_MODE_TICKS;
        }
    }
}

// Resets all ghosts to initial mode (scatter with full timer)
void ghosts_reset_mode(ghost_t *ghosts[GHOST_COUNT], int count) {
    ghost_mode_phase = 0;

    for (int i = 0; i < count; i++) {
        if (ghosts[i]) {
            ghosts[i]->mode = MODE_PHASES[0].mode;
            ghosts[i]->mode_ticks_remaining = MODE_PHASES[0].ticks;
        }
    }
}

/* ===================== */
/*   COLLISION           */
/* ===================== */

bool ghost_collides_with_player(const ghost_t *ghost, const player_t *player) {
    if (!ghost || !player) return false;

    // Dead ghosts cannot interact with the player
    if (ghost->state == GHOST_DEAD) return false;

    int ghost_row, ghost_col;
    int player_row, player_col;

    map_tile_from_pixel(ghost->x + TILE_SIZE / 2, ghost->y + TILE_SIZE / 2, &ghost_row, &ghost_col);
    player_get_tile(player, &player_row, &player_col);

    return ghost_row == player_row && ghost_col == player_col;
}

int ghosts_check_collision(ghost_t *ghosts[GHOST_COUNT], int count, const player_t *player) {
    for (int i = 0; i < count; i++) {
        if (ghosts[i] && ghost_collides_with_player(ghosts[i], player))
            return i;  // return index so caller knows which ghost was hit
    }
    return -1;  // no collision
}
