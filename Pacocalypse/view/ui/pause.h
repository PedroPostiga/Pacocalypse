#ifndef _PAUSE_H_
#define _PAUSE_H_

#include "../../view/button.h"
#include "../../view/font.h"
#include "../../view/resources/resources.h"

/**
 * @brief View state for the pause overlay buttons.
 */
typedef struct {
    button_t paused_button; /**< Button used to resume gameplay. */
    button_t quit_button;   /**< Button used to return to the main menu. */
} pause_state_t;

/**
 * @brief Initializes pause overlay button geometry and styling.
 *
 * @param pause Pause state to initialize.
 * @param font Font used for button text.
 */
void pause_init(pause_state_t *pause, font_t *font);

/**
 * @brief Draws the pause overlay buttons.
 *
 * @param pause Pause state to draw.
 * @param mouse_x Current mouse x coordinate for hover state.
 * @param mouse_y Current mouse y coordinate for hover state.
 * @param sprites Loaded sprite resources.
 */
void pause_draw(const pause_state_t *pause, int mouse_x, int mouse_y, const game_sprites_t *sprites);

#endif // _PAUSE_H_
