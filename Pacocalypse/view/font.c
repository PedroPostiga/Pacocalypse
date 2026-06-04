#include "font.h"
#include "../../../lab5/videocard.h"
#include <lcom/lcf.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

static void font_init_map(font_t *font) {
// mark everything as invalid
    for (int i = 0; i < 256; i++) {
        font->char_map[i] = UINT32_MAX;
    }

// Space character (map to invalid index to skip rendering)
    font->char_map[' '] = UINT32_MAX;

// A-Z
    for (int i = 0; i < 26; i++) {
        font->char_map['A' + i] = i;
    }

// a-z (map to uppercase)
    for (int i = 0; i < 26; i++) {
        font->char_map['a' + i] = i;
    }
}

static void font_init_sheet_map(font_t *font) {
    uint32_t second_row = font->tiles_per_row;

// 0-9
    for (int i = 0; i < 10; i++) {
        font->char_map['0' + i] = second_row + i;
    }

// punctuation (must match sprite sheet order!)
    font->char_map['.'] = second_row + 10;
    font->char_map[':'] = second_row + 11;
    font->char_map['!'] = second_row + 12;
    font->char_map['?'] = second_row + 13;
    font->char_map['+'] = second_row + 14;
    font->char_map['-'] = second_row + 15;
    font->char_map['/'] = second_row + 16;
    font->char_map[' '] = second_row + 17; // space character (empty tile)

}

    font_t* font_create(uint32_t tile_size, xpm_map_t xpm) {
    if (!xpm || tile_size == 0) return NULL;

    font_t *font = malloc(sizeof(font_t));
    if (!font) return NULL;

    font_init_map(font);

    sprite_t *full_font = sprite_create(xpm);
    if (!full_font) {
        free(font);
        return NULL;
    }

    font->tile_size = tile_size;
    font->tiles_per_row = full_font->width / tile_size;
    font->num_tiles =
        (full_font->height / tile_size) * (full_font->width / tile_size);
    font_init_sheet_map(font);

    font->tiles = malloc(font->num_tiles * sizeof(sprite_t*));
    if (!font->tiles) {
        sprite_destroy(full_font);
        free(font);
        return NULL;
    }

    for (uint32_t i = 0; i < font->num_tiles; i++) {
        uint16_t tile_row = i / font->tiles_per_row;
        uint16_t tile_col = i % font->tiles_per_row;

        int src_x = tile_col * tile_size;
        int src_y = tile_row * tile_size;

        font->tiles[i] = malloc(sizeof(sprite_t));
        if (!font->tiles[i]) {
            for (uint32_t j = 0; j < i; j++) {
                free(font->tiles[j]->pixmap);
                free(font->tiles[j]);
            }
            free(font->tiles);
            sprite_destroy(full_font);
            free(font);
            return NULL;
        }

        font->tiles[i]->width = tile_size;
        font->tiles[i]->height = tile_size;
        font->tiles[i]->pixmap = malloc(tile_size * tile_size);

        if (!font->tiles[i]->pixmap) {
            for (uint32_t j = 0; j <= i; j++) {
                if (j < i) free(font->tiles[j]->pixmap);
                free(font->tiles[j]);
            }
            free(font->tiles);
            sprite_destroy(full_font);
            free(font);
            return NULL;
        }

        for (uint16_t y = 0; y < tile_size; y++) {
            for (uint16_t x = 0; x < tile_size; x++) {
                uint8_t pixel = full_font->pixmap[(src_y + y) * full_font->width + (src_x + x)];
                font->tiles[i]->pixmap[y * tile_size + x] = pixel;
            }
        }
    }

    sprite_destroy(full_font);
    return font;

}

void font_destroy(font_t* font) {
    if (!font) return;

    if (font->tiles) {
        for (uint32_t i = 0; i < font->num_tiles; i++) {
            if (font->tiles[i]) {
                free(font->tiles[i]->pixmap);
                free(font->tiles[i]);
            }
        }
        free(font->tiles);
    }

free(font);

}

int draw_char(font_t* font, char c, int x, int y) {
    if (!font || !font->tiles) return -1;

    uint32_t char_index = font->char_map[(uint8_t)c];
    if (char_index == UINT32_MAX || char_index >= font->num_tiles)
        return -1;

    sprite_t *ch = font->tiles[char_index];
    if (!ch || !ch->pixmap) return -1;

    for (int j = 0; j < ch->height; j++) {
        for (int i = 0; i < ch->width; i++) {
            uint8_t pixel = ch->pixmap[j * ch->width + i];
            if (pixel != 0) {
                vg_draw_pixel(x + i, y + j, pixel);
            }
        }
    }

    return 0;
}

int draw_string(font_t* font, const char* text, int x, int y) {
    if (!font || !text) return -1;

    int cx = x;

    for (int i = 0; text[i] != '\0'; i++) {
        if (draw_char(font, text[i], cx, y) != 0)
            return -1;

        cx += font->tile_size;
    }

    return 0;

}


