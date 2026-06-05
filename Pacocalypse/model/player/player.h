#ifndef _PLAYER_H_
#define _PLAYER_H_

#include <stdint.h>
#include <stdbool.h>
#include "../map/map.h"

/* ===================== */
/*   PLAYER CONSTANTS    */
/* ===================== */

/** @brief Number of lives the player starts with. */
#define PLAYER_LIVES        3

/** @brief Player movement speed in pixels per tick. */
#define PLAYER_SPEED        2

/** @brief Number of frames in each player animation. */
#define PLAYER_ANIM_FRAMES  3

/** @brief Number of ticks between player animation frame changes. */
#define PLAYER_ANIM_SPEED   8

/* ===================== */
/*   DIRECTION           */
/* ===================== */

/**
 * @brief Cardinal movement directions used by player and ghosts.
 */
typedef enum {
    DIR_NONE,  /**< No movement direction. */
    DIR_UP,    /**< Move upward. */
    DIR_DOWN,  /**< Move downward. */
    DIR_LEFT,  /**< Move left. */
    DIR_RIGHT  /**< Move right. */
} direction_t;

/* ===================== */
/*   PLAYER STRUCT       */
/* ===================== */

/**
 * @brief Mutable state for the player model.
 */
typedef struct {
    int x;                              /**< Pixel x position of the sprite top-left corner. */
    int y;                              /**< Pixel y position of the sprite top-left corner. */
    direction_t direction;              /**< Current movement direction. */
    direction_t next_direction;         /**< Buffered direction requested by input. */
    uint8_t lives;                      /**< Remaining lives. */
    uint32_t score;                     /**< Current score. */
    bool powered_up;                    /**< Whether the player has at least one usable power-up. */
    uint8_t power_ups_available;        /**< Stored power-up charges that can still be used. */
    uint32_t power_up_ticks_remaining;  /**< Compatibility field for older timed power-up callers. */
    uint8_t anim_frame;                 /**< Current animation frame index. */
    uint8_t anim_tick_counter;          /**< Ticks since last animation frame change. */
    bool alive;                         /**< Whether the player is alive in the current life. */
} player_t;

/* ===================== */
/*   LIFECYCLE           */
/* ===================== */

/**
 * @brief Allocates and initializes a player at the map's player spawn tile.
 *
 * @param map Map used to find the player spawn tile.
 * @return Created player, or NULL on failure.
 */
player_t *player_create(const map_t *map);

/**
 * @brief Frees all memory associated with the player.
 *
 * @param player Player to destroy. NULL is ignored.
 */
void player_destroy(player_t *player);

/* ===================== */
/*   PER-TICK UPDATES    */
/* ===================== */

/**
 * @brief Advances player movement by one tick.
 *
 * Attempts the buffered direction first when valid, then falls back to the
 * current direction.
 *
 * @param player Player to move.
 * @param map Map used for collision and portal checks.
 */
void player_move(player_t *player, map_t *map);

/**
 * @brief Advances the player animation counter by one tick.
 *
 * @param player Player to update.
 */
void player_tick_animation(player_t *player);

/**
 * @brief Updates the cached powered_up flag from the stored power-up count.
 *
 * @param player Player to update.
 */
void player_tick_power_up(player_t *player);

/* ===================== */
/*   INPUT               */
/* ===================== */

/**
 * @brief Buffers a new desired movement direction.
 *
 * @param player Player to mutate.
 * @param dir Desired direction.
 */
void player_set_direction(player_t *player, direction_t dir);

/* ===================== */
/*   GAME EVENTS         */
/* ===================== */

/**
 * @brief Collects the tile under the player if it contains a collectible.
 *
 * @param player Player collecting the tile.
 * @param map Map to mutate.
 * @return Collected tile type, or TILE_EMPTY if nothing was collected.
 */
tile_type_t player_collect(player_t *player, map_t *map);

/**
 * @brief Stores one newly collected power-up.
 *
 * @param player Player receiving the power-up charge.
 */
void player_activate_power_up(player_t *player);

/**
 * @brief Consumes one stored power-up if available.
 *
 * @param player Player using a power-up charge.
 * @return true if a power-up was consumed, false otherwise.
 */
bool player_use_power_up(player_t *player);

/**
 * @brief Handles player death and resets life-specific state.
 *
 * @param player Player to reset.
 * @param map Map used to find the spawn tile.
 * @return true if the player still has lives remaining, false on game over.
 */
bool player_die(player_t *player, const map_t *map);

/* ===================== */
/*   SCORE               */
/* ===================== */

/** @brief Score awarded for collecting one pellet. */
#define SCORE_PELLET        10

/** @brief Score awarded for collecting one power-up. */
#define SCORE_POWER_UP      50

/** @brief Score awarded for eating one frightened ghost. */
#define SCORE_GHOST_EAT     200

/**
 * @brief Adds points to the player's score.
 *
 * @param player Player to mutate.
 * @param points Points to add.
 */
void player_add_score(player_t *player, uint32_t points);

/* ===================== */
/*   HELPERS             */
/* ===================== */

/**
 * @brief Returns the tile row and column occupied by the player's center.
 *
 * @param player Player to query.
 * @param row Output tile row.
 * @param col Output tile column.
 */
void player_get_tile(const player_t *player, int *row, int *col);

/**
 * @brief Checks whether the player overlaps a pixel rectangle.
 *
 * @param player Player to test.
 * @param x Rectangle left coordinate.
 * @param y Rectangle top coordinate.
 * @param w Rectangle width.
 * @param h Rectangle height.
 * @return true if the player's bounding box overlaps the rectangle.
 */
bool player_collides_with(const player_t *player, int x, int y, int w, int h);

#endif // _PLAYER_H_
