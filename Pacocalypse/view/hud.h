#ifndef _HUD_H_
#define _HUD_H_

#include "../model/player/player.h"
#include "font.h"

/**
 * @brief Draws the in-game HUD.
 * 
 * Shows score, lives, alive time, and stored power-up count.
 *
 * @param player Player data to display.
 * @param font Font used for drawing text.
 * @param alive_seconds Number of seconds survived in the current run.
 */
void hud_draw(const player_t *player, font_t *font, uint32_t alive_seconds);

#endif // _HUD_H_
