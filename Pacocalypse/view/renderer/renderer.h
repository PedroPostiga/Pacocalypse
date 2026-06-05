#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "../../model/game/game.h"
#include "../resources/resources.h"
#include "../ui/menu.h"
#include "../ui/pause.h"

int renderer_init(void);

int renderer_cleanup(void);

void renderer_update_animations(const game_state_t *state, const view_resources_t *resources);

void renderer_draw_game(const game_state_t* state,
                        const view_resources_t *resources,
                        int mouse_x,
                        int mouse_y,
                        const menu_state_t *menu,
                        const pause_state_t *pause);

int draw_sprite(const sprite_t* sprite, int x, int y);

int draw_animated_sprite(const animated_sprite_t* sprite);

void renderer_draw_button(const button_t *btn, int mouse_x, int mouse_y);

#endif // _RENDERER_H_
