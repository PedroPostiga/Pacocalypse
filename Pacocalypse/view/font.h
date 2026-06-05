#ifndef _FONT_H_

#define _FONT_H_



#include <lcom/lcf.h>

#include <stdint.h>

#include <stdlib.h>

#include "sprite.h"



/**
 * @brief Bitmap font loaded from a sprite sheet.
 */
typedef struct {

    sprite_t **tiles;        /**< Glyph sprites indexed by tile number. */

    uint16_t tile_size;      /**< Width and height of each square glyph tile. */

    uint32_t num_tiles;      /**< Number of glyph tiles available. */

    uint16_t tiles_per_row;  /**< Number of glyph tiles per sprite-sheet row. */

    uint32_t char_map[256]; /**< Map ASCII characters to tile indices. */

} font_t;



/**
 * @brief Creates a bitmap font from an XPM sprite sheet.
 *
 * @param tile_size Size in pixels of each square glyph.
 * @param xpm XPM map containing the glyph sheet.
 * @return Pointer to the created font, or NULL on failure.
 */
font_t* font_create(uint32_t tile_size, xpm_map_t xpm);



/**
 * @brief Frees all memory owned by a font.
 *
 * @param font Font to destroy. NULL is ignored.
 */
void font_destroy(font_t* font);



/**
 * @brief Draws a single character at a screen position.
 *
 * @param font Font used for drawing.
 * @param c Character to draw.
 * @param x Left screen coordinate.
 * @param y Top screen coordinate.
 * @return 0 on success, non-zero on failure.
 */
int draw_char(font_t* font, char c, int x, int y);



/**
 * @brief Draws a null-terminated string at a screen position.
 *
 * @param font Font used for drawing.
 * @param text Text to draw.
 * @param x Left screen coordinate.
 * @param y Top screen coordinate.
 * @return 0 on success, non-zero on failure.
 */
int draw_string(font_t* font, const char* text, int x, int y);

#endif // _FONT_H_
