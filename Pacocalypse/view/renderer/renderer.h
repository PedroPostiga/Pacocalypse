#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "../../model/game/game.h"

int renderer_init(void);

int renderer_cleanup(void);

void renderer_draw_game(const game_state_t* state);

int draw_sprite(const sprite_t* sprite, int x, int y);

int draw_animated_sprite(const animated_sprite_t* sprite);

#endif // _RENDERER_H_
