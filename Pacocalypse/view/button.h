#ifndef _BUTTON_H_
#define _BUTTON_H_

#include <stdbool.h>
#include <string.h>
#include "font.h"
#include "sprite.h"

typedef enum {
    MILD_GREEN = 0x00008800,
    DARK_GRAY = 0x333333,
    CRIMSON_RED = 0xaa0000
} button_color_t;

typedef struct {
    int x; // Added coordinates since sprite_t doesn't have them
    int y;
    int width;
    int height;
    sprite_t *sp;
    sprite_t *hover_sp;
    char text[100];
    font_t *font;
    button_color_t back_color;
    button_color_t hover_frame_color;
} button_t;

/**
 * @brief Draws the button on the screen
 * 
 * @param b pointer to the button_t structure
 * @param hover true if the button is currently being hovered
 */
void button_draw(const button_t *b, bool hover);

/**
 * @brief Checks if the mouse coordinates are inside the button's boundaries
 * 
 * @param b pointer to the button_t structure
 * @param mouse_x the x coordinate of the mouse
 * @param mouse_y the y coordinate of the mouse
 * @return true if mouse is hovering over the button, false otherwise
 */
bool button_is_hovered(const button_t *b, int mouse_x, int mouse_y);

#endif // _BUTTON_H_
