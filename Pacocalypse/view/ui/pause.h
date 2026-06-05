#ifndef _PAUSE_H_
#define _PAUSE_H_

#include "../../model/game/game.h"
#include "../../model/input/input.h"
#include "../../view/button.h"
#include "../../view/font.h"
#include "../../view/resources/resources.h"

typedef struct {
    button_t paused_button;
    button_t quit_button;
} pause_state_t;

void pause_init(pause_state_t *pause, font_t *font);
void pause_handle_mouse(pause_state_t *pause, const input_state_t *input, game_state_t *game_state);
void pause_draw(const pause_state_t *pause, const input_state_t *input, const game_sprites_t *sprites);

#endif // _PAUSE_H_
