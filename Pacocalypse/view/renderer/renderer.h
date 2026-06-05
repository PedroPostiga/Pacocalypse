#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "../../model/game/game.h"
#include "../resources/resources.h"
#include "../ui/menu.h"
#include "../ui/pause.h"

/**
 * @brief Initializes renderer state for the configured video mode.
 *
 * @return 0 on success, non-zero on failure.
 */
int renderer_init(void);

/**
 * @brief Cleans up renderer-owned state.
 *
 * @return 0 on success, non-zero on failure.
 */
int renderer_cleanup(void);

/**
 * @brief Advances view-side animation frames for visible entities.
 *
 * @param state Current game state.
 * @param resources Loaded view resources.
 */
void renderer_update_animations(const game_state_t *state, const view_resources_t *resources);

/**
 * @brief Draws the current game screen based on the active game mode.
 *
 * @param state Current game state.
 * @param resources Loaded view resources.
 * @param mouse_x Mouse x coordinate used for cursor and hover states.
 * @param mouse_y Mouse y coordinate used for cursor and hover states.
 * @param menu Main menu view state.
 * @param pause Pause overlay view state.
 */
void renderer_draw_game(const game_state_t* state,
                        const view_resources_t *resources,
                        int mouse_x,
                        int mouse_y,
                        const menu_state_t *menu,
                        const pause_state_t *pause);

/**
 * @brief Draws a static sprite at a screen position.
 *
 * @param sprite Sprite to draw.
 * @param x Left screen coordinate.
 * @param y Top screen coordinate.
 * @return 0 on success, non-zero on failure.
 */
int draw_sprite(const sprite_t* sprite, int x, int y);

/**
 * @brief Draws an animated sprite's current frame at its stored position.
 *
 * @param sprite Animated sprite to draw.
 * @return 0 on success, non-zero on failure.
 */
int draw_animated_sprite(const animated_sprite_t* sprite);

/**
 * @brief Draws a button using the current hover state.
 *
 * @param btn Button to draw.
 * @param mouse_x Mouse x coordinate for hover testing.
 * @param mouse_y Mouse y coordinate for hover testing.
 */
void renderer_draw_button(const button_t *btn, int mouse_x, int mouse_y);

#endif // _RENDERER_H_
