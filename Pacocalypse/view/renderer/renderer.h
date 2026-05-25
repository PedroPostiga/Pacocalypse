#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "../../model/game/game.h"

int renderer_init(void);
void renderer_cleanup(void);
void renderer_draw_game(const game_state_t* state);

#endif // _RENDERER_H_
