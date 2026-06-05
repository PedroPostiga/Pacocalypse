#ifndef _MENU_H_
#define _MENU_H_

#include <stdbool.h>
#include "../../view/button.h"
#include "../../view/font.h"

/**
 * @brief View state for the main menu buttons.
 */
typedef struct {
    button_t play_button; /**< Button that starts a new game. */
    button_t quit_button; /**< Button that exits the program. */
} menu_state_t;

/**
 * @brief Initializes menu button geometry and styling.
 *
 * @param menu Menu state to initialize.
 * @param font Font used for button text.
 */
void menu_init(menu_state_t *menu, font_t *font);

/**
 * @brief Draws the menu background and buttons.
 *
 * @param menu Menu state to draw.
 * @param mouse_x Current mouse x coordinate for hover state.
 * @param mouse_y Current mouse y coordinate for hover state.
 * @param sprites Loaded sprite resources.
 */
void menu_draw(const menu_state_t *menu, int mouse_x, int mouse_y, const game_sprites_t *sprites);

#endif // _MENU_H_
