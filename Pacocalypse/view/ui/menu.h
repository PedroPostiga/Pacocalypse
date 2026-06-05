#ifndef _MENU_H_
#define _MENU_H_

#include <stdbool.h>
#include "../../view/button.h"
#include "../../view/font.h"
#include "../../model/game/game.h"
#include "../../model/input/input.h"

typedef struct {
    button_t play_button;
    button_t quit_button;
} menu_state_t;

void menu_init(menu_state_t *menu, font_t *font);
void menu_handle_mouse(menu_state_t *menu, const input_state_t *input, game_state_t *game_state);
void menu_draw(const menu_state_t *menu, const input_state_t *input, const game_sprites_t *sprites);

#endif // _MENU_H_
