#ifndef _PAUSE_H_
#define _PAUSE_H_

#include "../../view/button.h"
#include "../../view/font.h"
#include "../../view/resources/resources.h"

typedef struct {
    button_t paused_button;
    button_t quit_button;
} pause_state_t;

void pause_init(pause_state_t *pause, font_t *font);
void pause_draw(const pause_state_t *pause, int mouse_x, int mouse_y, const game_sprites_t *sprites);

#endif // _PAUSE_H_
