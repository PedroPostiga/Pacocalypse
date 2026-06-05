#ifndef _HUD_H_
#define _HUD_H_

#include "../model/player/player.h"
#include "model/map/map.h"
#include "font.h"

/**
 * @brief Draws the HUD (Heads Up Display) showing score and lives
 * 
 * @param player pointer to the player structure containing score and lives
 * @param font pointer to the font used for drawing
 */
void hud_draw(const player_t *player, font_t *font, uint32_t alive_seconds);

#endif // _HUD_H_
