#ifndef _FONT_H_

#define _FONT_H_



#include <lcom/lcf.h>

#include <stdint.h>

#include <stdlib.h>

#include "../sprite.h"



typedef struct {

    sprite_t **tiles;

    uint16_t tile_size;

    uint32_t num_tiles;

    uint16_t tiles_per_row;

    uint32_t char_map[256]; // Map ASCII characters to tile indices

} font_t;



font_t* font_create(uint32_t tile_size, xpm_map_t xpm);



void font_destroy(font_t* font);



int draw_char(font_t* font, char c, int x, int y);



int draw_string(font_t* font, const char* text, int x, int y);

#endif // _FONT_H_
