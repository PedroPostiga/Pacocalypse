#ifndef _GAME_H_
#define _GAME_H_

#include "../map/map.h"
#include "../player/player.h"
#include "../ghost/ghost.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/**
 * @brief High-level mode of the game/application.
 */
typedef enum {
    STATE_MENU,      /**< Main menu is active. */
    STATE_PLAYING,   /**< Gameplay is active. */
    STATE_PAUSED,    /**< Gameplay is paused. */
    STATE_GAME_OVER, /**< Legacy game-over mode. */
    STATE_QUIT       /**< Application should exit. */
} game_mode_t;

/**
 * @brief Complete mutable state of one game session/application run.
 */
typedef struct {
    game_mode_t mode;                  /**< Current game/application mode. */
    player_t *player;                  /**< Player model. */
    map_t *map;                        /**< Tile map model. */
    ghost_t *ghosts[GHOST_COUNT];      /**< Active ghost models. */
    int num_ghosts;                    /**< Number of valid ghosts in ghosts[]. */
    uint32_t run_alive_seconds;        /**< Seconds survived in the current/last run. */
    uint32_t run_start_seconds;        /**< Start time in seconds since midnight. */
    uint8_t run_tick_counter;          /**< Timer fallback counter for alive time. */
    bool run_timer_started;            /**< Whether alive-time tracking has started. */
    bool run_summary_available;        /**< Whether the menu should show last-run summary data. */
} game_state_t;

/**
 * @brief Initializes the game model and allocates owned state.
 *
 * @param state Game state to initialize.
 * @return 0 on success, non-zero on failure.
 */
int game_init(game_state_t* state);

/**
 * @brief Recreates the full game model for a new run.
 *
 * @param state Game state to restart.
 * @return 0 on success, non-zero on failure.
 */
int game_restart(game_state_t* state);

/**
 * @brief Frees all model objects owned by the game state.
 *
 * @param state Game state to clean up.
 */
void game_cleanup(game_state_t* state);

/**
 * @brief Applies a mouse click as a possible power-up activation.
 *
 * @param state Current game state.
 * @param mouse_x Click x coordinate.
 * @param mouse_y Click y coordinate.
 * @param left_click true only on the frame a left click begins.
 */
void game_handle_power_up_click(game_state_t* state, int mouse_x, int mouse_y, bool left_click);

/**
 * @brief Advances gameplay by one update tick.
 *
 * @param state Game state to update.
 * @return 0 on success, non-zero on failure.
 */
int game_update(game_state_t* state);

#endif // _GAME_H_
