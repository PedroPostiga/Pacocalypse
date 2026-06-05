#ifndef _CONTROL_H
#define _CONTROL_H

#include <lcom/lcf.h>
#include <stdbool.h>

#include "../../../lab2/timer.h"
#include "../../../lab3/kbc.h"
#include "../../../lab4/mouse.h"
#include "../game/game.h"
#include "../input/input.h"

void update_keyboard_state(game_state_t *state);
bool update_mouse_state(input_state_t *input);
int update_timer_state(game_state_t *state);

#endif
