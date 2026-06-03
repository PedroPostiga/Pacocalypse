#ifndef _PLAYER_H_
#define _PLAYER_H_

#include <stdint.h>
#include <stdbool.h>
#include "../map/map.h"

/* ===================== */
/*   PLAYER CONSTANTS    */
/* ===================== */

#define PLAYER_LIVES        3
#define PLAYER_SPEED        2           // Pixels per tick
#define PLAYER_POWER_UP_DURATION 180    // Ticks (3 seconds at 60 Hz)
#define PLAYER_ANIM_FRAMES  3           // Number of animation frames
#define PLAYER_ANIM_SPEED   8           // Ticks per animation frame

/* ===================== */
/*   DIRECTION           */
/* ===================== */

typedef enum {
    DIR_NONE,
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT
} direction_t;

/* ===================== */
/*   PLAYER STRUCT       */
/* ===================== */

typedef struct {
    int x;                          // Pixel x position (top-left of sprite)
    int y;                          // Pixel y position (top-left of sprite)
    direction_t direction;          // Current movement direction
    direction_t next_direction;     // Queued direction (buffered from keyboard)
    uint8_t lives;                  // Remaining lives
    uint32_t score;                 // Current score
    bool powered_up;                // Whether the power-up is active
    uint32_t power_up_ticks_remaining; // Countdown in ticks
    uint8_t anim_frame;             // Current animation frame index
    uint8_t anim_tick_counter;      // Ticks since last frame change
    bool alive;                     // Whether player is alive this life
} player_t;

/* ===================== */
/*   LIFECYCLE           */
/* ===================== */

/**
 * Allocates and initialises a player at the TILE_PLAYER_SPAWN position
 * found in the provided map. Returns NULL on failure.
 */
player_t *player_create(const map_t *map);

/**
 * Frees all memory associated with the player.
 */
void player_destroy(player_t *player);

/* ===================== */
/*   PER-TICK UPDATES    */
/* ===================== */

/**
 * Attempts to move the player in next_direction first, then current direction.
 * Only moves if the destination tile is walkable. Updates direction accordingly.
 */
void player_move(player_t *player, map_t *map);

/**
 * Advances the animation frame counter by one tick.
 */
void player_tick_animation(player_t *player);

/**
 * Decrements the power-up timer by one tick.
 * Deactivates the power-up when the counter reaches zero.
 */
void player_tick_power_up(player_t *player);

/* ===================== */
/*   INPUT               */
/* ===================== */

/**
 * Buffers a new desired direction; applied on the next move tick
 * if the resulting tile is walkable.
 */
void player_set_direction(player_t *player, direction_t dir);

/* ===================== */
/*   GAME EVENTS         */
/* ===================== */

/**
 * Checks the tile the player currently occupies and collects it if it is a
 * pellet or power-up. Adds to score and activates the power-up when needed.
 * Returns the type of tile collected (TILE_EMPTY if nothing was collected).
 */
tile_type_t player_collect(player_t *player, map_t *map);

/**
 * Activates the power-up, setting powered_up = true and resetting the
 * countdown to PLAYER_POWER_UP_DURATION.
 */
void player_activate_power_up(player_t *player);

/**
 * Handles player death: decrements lives, resets position to spawn,
 * clears direction and power-up state.
 * Returns true if the player still has lives remaining, false if game over.
 */
bool player_die(player_t *player, const map_t *map);

/* ===================== */
/*   SCORE               */
/* ===================== */

#define SCORE_PELLET        10
#define SCORE_POWER_UP      50
#define SCORE_GHOST_EAT     200  // Per ghost eaten while powered up

/**
 * Adds `points` to the player's score.
 */
void player_add_score(player_t *player, uint32_t points);

/* ===================== */
/*   HELPERS             */
/* ===================== */

/**
 * Returns the tile row and column that the player's centre occupies.
 */
void player_get_tile(const player_t *player, int *row, int *col);

/**
 * Returns true if the player's bounding box overlaps the given pixel rect.
 * Used for collision detection with ghosts.
 */
bool player_collides_with(const player_t *player, int x, int y, int w, int h);

#endif // _PLAYER_H_
