#ifndef _CONTROL_H
#define _CONTROL_H

#include <lcom/lcf.h>
#include <stdbool.h>

#include "../../config.h"
#include "../../../lab2/timer.h"
#include "../../../lab3/kbc.h"
#include "../../../lab4/mouse.h"
#include "../../../lab1/rtc.h"
#include "../../model/game/game.h"
#include "../input/input.h"
#include "../../view/renderer/renderer.h"
#include "../../view/resources/resources.h"
#include "../../view/ui/menu.h"
#include "../../view/ui/pause.h"

/**
 * @brief Handles one keyboard interrupt and applies keyboard commands.
 *
 * @param state Game state to mutate.
 * @return true if the game should quit, false otherwise.
 */
bool update_keyboard_state(game_state_t *state);

/**
 * @brief Handles one mouse interrupt and applies mouse-driven commands.
 *
 * @param state Game state to mutate.
 * @param input Input state to update.
 * @param menu Menu view state used for button hit testing.
 * @param pause Pause view state used for button hit testing.
 * @return true if the game should quit, false otherwise.
 */
bool update_mouse_state(game_state_t *state,
                        input_state_t *input,
                        menu_state_t *menu,
                        pause_state_t *pause);

/**
 * @brief Handles one timer interrupt, updates the model, and redraws the view.
 *
 * @param state Game state to update.
 * @param resources Loaded view resources.
 * @param input Current input state.
 * @param menu Menu view state.
 * @param pause Pause view state.
 * @return true if the game should quit, false otherwise.
 */
bool update_timer_state(game_state_t *state,
                        const view_resources_t *resources,
                        const input_state_t *input,
                        const menu_state_t *menu,
                        const pause_state_t *pause);

#endif
