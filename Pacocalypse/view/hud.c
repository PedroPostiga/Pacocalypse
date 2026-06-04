#include "hud.h"
#include <stdio.h>
#include <string.h>


void hud_draw(const player_t *player, font_t *font) {
    if (!player || !font) return;

    // Buffer to hold our formatted text
    char text_buffer[64];

    // Format and draw SCORE at the top left corner
    sprintf(text_buffer, "SCORE: %u", player->score);
    draw_string(font, text_buffer, 20, 20);

    // Format LIVES text
    sprintf(text_buffer, "LIVES: %u", player->lives);
    
    // Calculate width of the text to align it to the top right corner
    // Each character's width corresponds to font->tile_size
    int text_length = strlen(text_buffer);
    int text_width = text_length * font->tile_size;
    int right_x = SCREEN_WIDTH - text_width - 20;

    // Draw LIVES at the top right corner
    draw_string(font, text_buffer, right_x, 20);
}
