#ifndef _BUTTON_H_
#define _BUTTON_H_

#include <stdbool.h>
#include <string.h>
#include "font.h"
#include "sprite.h"

/**
 * @brief Button color constants used by the UI.
 */
typedef enum {
    MILD_GREEN = 0x00008800, /**< Muted green button color. */
    DARK_GRAY = 0x333333,    /**< Dark gray button/frame color. */
    CRIMSON_RED = 0xaa0000   /**< Red button color. */
} button_color_t;

/**
 * @brief Rectangular UI button with optional sprites and text.
 */
typedef struct {
    int x;                              /**< Left screen coordinate. */
    int y;                              /**< Top screen coordinate. */
    int width;                          /**< Button width in pixels. */
    int height;                         /**< Button height in pixels. */
    sprite_t *sp;                       /**< Optional default button sprite. */
    sprite_t *hover_sp;                 /**< Optional hover button sprite. */
    char text[100];                     /**< Button label. */
    font_t *font;                       /**< Font used for the label. */
    button_color_t back_color;          /**< Background color when no sprite is used. */
    button_color_t hover_frame_color;   /**< Frame color used while hovered. */
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
