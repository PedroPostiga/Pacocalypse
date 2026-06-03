#ifndef _CONTROL_H
#define _CONTROL_H

#include <lcom/lcf.h>
#include "controller/keyboard/kbc.h"
#include "controller/mouse/mouse.h"
#include "controller/timer/timer.h"
#include "controller/videocard/videocard.h"
#include "model/game/game.h"
#include "view/renderer/renderer.h"
#include "../../../lab1/rtc.h"

void update_keyboard_state(player_t *player);
void update_mouse_state(game_state_t *state);
void update_timer_state(game_state_t *state);
void update_rtc_state();

#endif 