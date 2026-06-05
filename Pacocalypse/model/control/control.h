#ifndef _CONTROL_H
#define _CONTROL_H

#include <lcom/lcf.h>
#include <stdbool.h>

#include "../../../lab2/timer.h"
#include "../../../lab3/kbc.h"
#include "../../../lab4/mouse.h"
#include "../game/game.h"
#include "../input/input.h"
#include "../../view/renderer/renderer.h"
#include "../../view/resources/resources.h"
#include "../../view/ui/menu.h"
#include "../../view/ui/pause.h"

bool update_keyboard_state(game_state_t *state);
bool update_mouse_state(game_state_t *state,
                        input_state_t *input,
                        menu_state_t *menu,
                        pause_state_t *pause);
bool update_timer_state(game_state_t *state,
                        const view_resources_t *resources,
                        const input_state_t *input,
                        const menu_state_t *menu,
                        const pause_state_t *pause);

#endif
