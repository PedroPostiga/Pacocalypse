#ifndef _GHOST_H_
#define _GHOST_H_

#include <stdint.h>
#include <stdbool.h>
#include "../map/map.h"
#include "../player/player.h"

/* ===================== */
/*   GHOST CONSTANTS     */
/* ===================== */

/** @brief Ghost movement speed in pixels per tick. */
#define GHOST_SPEED             2
/** @brief Number of ghosts in the game. */
#define GHOST_COUNT             4
/** @brief Number of frames in each normal ghost animation. */
#define GHOST_ANIM_FRAMES       2
/** @brief Number of ticks between ghost animation frame changes. */
#define GHOST_ANIM_SPEED        8
/** @brief Duration of frightened ghost state in ticks. */
#define GHOST_FRIGHTENED_TICKS  180
/** @brief Delay in ticks before eaten ghosts begin returning to spawn. */
#define GHOST_DEAD_TICKS        90
/** @brief Default chase mode duration in ticks. */
#define GHOST_CHASE_MODE_TICKS  1200
/** @brief Default scatter mode duration in ticks. */
#define GHOST_SCATTER_MODE_TICKS 420

/* ===================== */
/*   GHOST MODE          */
/* ===================== */

/**
 * @brief Global AI mode used by living ghosts.
 */
typedef enum {
    GHOST_MODE_CHASE,  /**< Ghosts target the player using their personality logic. */
    GHOST_MODE_SCATTER /**< Ghosts target their assigned corners. */
} ghost_mode_t;

/* ===================== */
/*   GHOST STATE         */
/* ===================== */

/**
 * @brief Life-cycle state of a ghost.
 */
typedef enum {
    GHOST_ALIVE,      /**< Normal behavior; targeting depends on ghost_mode_t. */
    GHOST_FRIGHTENED, /**< Ghost can be eaten by the player. */
    GHOST_DEAD,       /**< Ghost was eaten and is waiting before respawn movement. */
    GHOST_RESPAWNING  /**< Ghost eyes are returning to spawn. */
} ghost_state_t;

/* ===================== */
/*   GHOST IDENTITY      */
/* ===================== */

/**
 * @brief Ghost identity used for sprite selection and targeting personality.
 */
typedef enum {
    GHOST_RED    = 0, /**< Red ghost. */
    GHOST_PINK   = 1, /**< Pink ghost. */
    GHOST_CYAN   = 2, /**< Cyan/blue ghost. */
    GHOST_ORANGE = 3  /**< Orange ghost. */
} ghost_id_t;

/* ===================== */
/*   GHOST STRUCT        */
/* ===================== */

/**
 * @brief Mutable state for one ghost.
 */
typedef struct {
    int x;                          /**< Pixel x position of the sprite top-left corner. */
    int y;                          /**< Pixel y position of the sprite top-left corner. */
    direction_t direction;          /**< Current movement direction. */
    ghost_state_t state;            /**< Current life-cycle state. */
    ghost_id_t id;                  /**< Ghost identity/personality. */
    ghost_mode_t mode;              /**< Current chase/scatter targeting mode. */
    uint32_t state_ticks_remaining; /**< Countdown for temporary states. */
    uint32_t mode_ticks_remaining;  /**< Countdown for chase/scatter switching. */
    uint8_t anim_frame;             /**< Current animation frame index. */
    uint8_t anim_tick_counter;      /**< Ticks since last animation frame change. */
    int spawn_x;                    /**< Pixel x coordinate of this ghost's spawn tile. */
    int spawn_y;                    /**< Pixel y coordinate of this ghost's spawn tile. */
} ghost_t;

/* ===================== */
/*   LIFECYCLE           */
/* ===================== */

/**
 * @brief Allocates and initializes one ghost.
 *
 * @param x Initial pixel x coordinate.
 * @param y Initial pixel y coordinate.
 * @param id Ghost identity/personality.
 * @return Created ghost, or NULL on failure.
 */
ghost_t *ghost_create(int x, int y, ghost_id_t id);

/**
 * @brief Frees one ghost.
 *
 * @param ghost Ghost to destroy. NULL is ignored.
 */
void ghost_destroy(ghost_t *ghost);

/**
 * @brief Creates all ghosts from TILE_GHOST_SPAWN tiles in the map.
 *
 * @param map Map containing ghost spawn tiles.
 * @param ghosts Output array filled with created ghosts.
 * @return Number of ghosts successfully created.
 */
int ghosts_create_all(const map_t *map, ghost_t *ghosts[GHOST_COUNT]);

/**
 * @brief Destroys all ghosts in an array.
 *
 * @param ghosts Ghost array to clean up.
 * @param count Number of entries to inspect.
 */
void ghosts_destroy_all(ghost_t *ghosts[GHOST_COUNT], int count);

/**
 * @brief Resets all ghosts to their spawn positions and initial mode.
 *
 * @param ghosts Ghost array to reset.
 * @param count Number of entries to inspect.
 */
void ghosts_reset_all(ghost_t *ghosts[GHOST_COUNT], int count);

/* ===================== */
/*   PER-TICK UPDATES    */
/* ===================== */

/**
 * @brief Advances one ghost by one tick without player targeting context.
 *
 * @param ghost Ghost to update.
 * @param map Map used for movement.
 */
void ghost_tick(ghost_t *ghost, const map_t *map);

/**
 * @brief Advances all ghosts by one tick without player targeting context.
 *
 * @param ghosts Ghost array to update.
 * @param count Number of entries to inspect.
 * @param map Map used for movement.
 */
void ghosts_tick_all(ghost_t *ghosts[GHOST_COUNT], int count, const map_t *map);

/**
 * @brief Advances all ghosts by one tick with player targeting context.
 *
 * @param ghosts Ghost array to update.
 * @param count Number of entries to inspect.
 * @param map Map used for movement.
 * @param player Player used by ghost AI targeting.
 */
void ghosts_tick_all_with_context(ghost_t *ghosts[GHOST_COUNT], int count, const map_t *map, const player_t *player);

/* ===================== */
/*   STATE CHANGES       */
/* ===================== */

/**
 * @brief Switches a ghost to frightened state.
 *
 * @param ghost Ghost to frighten.
 */
void ghost_frighten(ghost_t *ghost);

/**
 * @brief Switches all ghosts to frightened state.
 *
 * @param ghosts Ghost array to mutate.
 * @param count Number of entries to inspect.
 */
void ghosts_frighten_all(ghost_t *ghosts[GHOST_COUNT], int count);

/**
 * @brief Marks a frightened ghost as eaten and starts its respawn flow.
 *
 * @param ghost Ghost to eat.
 */
void ghost_eat(ghost_t *ghost);

/* ===================== */
/*   COLLISION           */
/* ===================== */

/**
 * @brief Checks whether a ghost and player occupy the same center tile.
 *
 * @param ghost Ghost to test.
 * @param player Player to test.
 * @return true if they should interact, false otherwise.
 */
bool ghost_collides_with_player(const ghost_t *ghost, const player_t *player);

/**
 * @brief Updates global ghost chase/scatter mode timing.
 *
 * @param ghosts Ghost array to mutate.
 * @param count Number of entries to inspect.
 * @return Current shared ghost mode.
 */
ghost_mode_t ghosts_update_mode(ghost_t *ghosts[GHOST_COUNT], int count);

/**
 * @brief Sets all ghosts to a specific mode immediately.
 *
 * @param ghosts Ghost array to mutate.
 * @param count Number of entries to inspect.
 * @param mode Mode to apply.
 */
void ghosts_set_mode(ghost_t *ghosts[GHOST_COUNT], int count, ghost_mode_t mode);

/**
 * @brief Resets all ghosts to the initial mode schedule.
 *
 * @param ghosts Ghost array to mutate.
 * @param count Number of entries to inspect.
 */
void ghosts_reset_mode(ghost_t *ghosts[GHOST_COUNT], int count);

#endif // _GHOST_H_
