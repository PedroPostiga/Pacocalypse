#ifndef _GHOST_H_
#define _GHOST_H_

#include <stdint.h>
#include <stdbool.h>
#include "../map/map.h"
#include "../player/player.h"

/* ===================== */
/*   GHOST CONSTANTS     */
/* ===================== */

#define GHOST_SPEED             2    // Pixels per tick
#define GHOST_COUNT             4    // Number of ghosts in the game
#define GHOST_ANIM_FRAMES       2    // Normal: 2 frames; frightened handled separately
#define GHOST_ANIM_SPEED        8    // Ticks per animation frame
#define GHOST_FRIGHTENED_TICKS  180  // Duration of frightened state (3s at 60 Hz)
#define GHOST_DEAD_TICKS        90   // Ticks before rushing to respawn (1.5s at 60 Hz)
#define GHOST_CHASE_MODE_TICKS  1200 // Main chase duration (20s at 60 Hz)
#define GHOST_SCATTER_MODE_TICKS 420 // Initial scatter duration (7s at 60 Hz)

/* ===================== */
/*   GHOST MODE          */
/* ===================== */

// Mode determines targeting behavior during chase state
typedef enum {
    GHOST_MODE_CHASE,   // Target player directly (or prediction)
    GHOST_MODE_SCATTER  // Target own corner (out of bounds)
} ghost_mode_t;

/* ===================== */
/*   GHOST STATE         */
/* ===================== */

// Behavioural state of a ghost. Keep historically-used enum names
// (GHOST_FRIGHTENED, GHOST_DEAD, ...) for compatibility with callers.
typedef enum {
    GHOST_ALIVE, // Normal behaviour; use `mode` to choose targeting
    GHOST_FRIGHTENED,        // Player has power-up active — ghost flees and can be eaten
    GHOST_DEAD,              // Eaten — waiting to respawn at ghost spawn tile
    GHOST_RESPAWNING         // Returning to spawn point at double speed
} ghost_state_t;

/* ===================== */
/*   GHOST IDENTITY      */
/* ===================== */

// Colour identity — used by renderer to pick the right sprite sheet
typedef enum {
    GHOST_RED   = 0,
    GHOST_PINK  = 1,
    GHOST_CYAN  = 2,
    GHOST_ORANGE = 3
} ghost_id_t;

/* ===================== */
/*   GHOST STRUCT        */
/* ===================== */

typedef struct {
    int x;                          // Pixel x position (top-left of sprite)
    int y;                          // Pixel y position (top-left of sprite)
    direction_t direction;          // Current movement direction
    ghost_state_t state;            // Current behavioural state
    ghost_id_t id;                  // Identity (colour)
    ghost_mode_t mode;              // Chase vs Scatter mode (only used during GHOST_CHASE state)
    uint32_t state_ticks_remaining; // Countdown for FRIGHTENED or DEAD states
    uint32_t mode_ticks_remaining;  // Countdown for mode switching
    uint8_t anim_frame;             // Current animation frame index
    uint8_t anim_tick_counter;      // Ticks since last frame change
    int spawn_x;                    // Pixel x of this ghost's spawn point (for respawn)
    int spawn_y;                    // Pixel y of this ghost's spawn point
} ghost_t;

/* ===================== */
/*   LIFECYCLE           */
/* ===================== */

/**
 * Allocates and initialises a ghost at the given pixel position.
 * `id` sets the ghost's colour identity.
 * Returns NULL on failure.
 */
ghost_t *ghost_create(int x, int y, ghost_id_t id);

/**
 * Frees all memory associated with the ghost.
 */
void ghost_destroy(ghost_t *ghost);

/**
 * Allocates and initialises all GHOST_COUNT ghosts placed at the
 * TILE_GHOST_SPAWN tiles found in the map. Fills `ghosts[]`.
 * Returns the number of ghosts successfully created (≤ GHOST_COUNT).
 */
int ghosts_create_all(const map_t *map, ghost_t *ghosts[GHOST_COUNT]);

/**
 * Destroys and frees all ghosts in the array.
 */
void ghosts_destroy_all(ghost_t *ghosts[GHOST_COUNT], int count);

/**
 * Resets all ghosts to their spawn positions and initial state (CHASE).
 * Called when the player loses a life, to reset the level without reloading the map. Does not free or reallocate memory.
 */
void ghosts_reset_all(ghost_t *ghosts[GHOST_COUNT], int count);

/* ===================== */
/*   PER-TICK UPDATES    */
/* ===================== */

/**
 * Advances the ghost by one tick: handles movement, state countdown,
 * and animation. Call once per ghost per timer interrupt.
 */
void ghost_tick(ghost_t *ghost, const map_t *map);

/**
 * Advances all ghosts by one tick.
 */
void ghosts_tick_all(ghost_t *ghosts[GHOST_COUNT], int count, const map_t *map);

/**
 * Advances all ghosts by one tick with full game context.
 * This version includes player targeting and AI decision-making.
 * Call this from game_update instead of ghosts_tick_all for proper ghost behavior.
 */
void ghosts_tick_all_with_context(ghost_t *ghosts[GHOST_COUNT], int count, const map_t *map, const player_t *player);

/* ===================== */
/*   STATE CHANGES       */
/* ===================== */

/**
 * Switches the ghost to GHOST_FRIGHTENED state and resets its countdown.
 * Called when the player collects a power-up.
 */
void ghost_frighten(ghost_t *ghost);

/**
 * Frightens all ghosts at once.
 */
void ghosts_frighten_all(ghost_t *ghosts[GHOST_COUNT], int count);

/**
 * Marks the ghost as eaten (GHOST_DEAD). Resets countdown for respawn.
 * Called by the game when the player touches a frightened ghost.
 */
void ghost_eat(ghost_t *ghost);

/* ===================== */
/*   COLLISION           */
/* ===================== */

/**
 * Returns true if this ghost's bounding box overlaps the player's.
 * Caller should check ghost->state to decide the outcome
 * (CHASE → player dies; FRIGHTENED → ghost eaten).
 */
bool ghost_collides_with_player(const ghost_t *ghost, const player_t *player);

/**
 * Updates the mode for all ghosts based on mode countdown timer.
 * Call this once per tick from game_update to manage chase/scatter transitions.
 * Returns the current mode (useful for display/debugging).
 */
ghost_mode_t ghosts_update_mode(ghost_t *ghosts[GHOST_COUNT], int count);

/**
 * Sets all ghosts to a specific mode immediately.
 * Resets the mode timer to the appropriate duration.
 */
void ghosts_set_mode(ghost_t *ghosts[GHOST_COUNT], int count, ghost_mode_t mode);

/**
 * Resets all ghosts' mode to initial state (GHOST_MODE_SCATTER with full timer).
 * Called when game restarts.
 */
void ghosts_reset_mode(ghost_t *ghosts[GHOST_COUNT], int count);

#endif // _GHOST_H_
