#include "button.h"
#include "renderer/renderer.h"
#include <lcom/lcf.h>

void button_draw(const button_t *b, bool hover) {
    // Select the appropriate sprite based on hover state
    sprite_t *curr_sp = hover ? b->hover_sp : b->sp;
    
    // Draw the sprite at the button's position, or a rectangle if sprite is NULL
    if (curr_sp != NULL) {
        draw_sprite(curr_sp, b->x, b->y);
    } else {
        uint32_t color = hover ? b->hover_frame_color : b->back_color;
        vg_draw_rectangle(b->x, b->y, b->width, b->height, color);
    }
    
    if (b->font != NULL) {
        // Calculate the width and height of the text based on tile size
        int text_width = strlen(b->text) * b->font->tile_size;
        int text_height = b->font->tile_size;
        
        // Calculate centered coordinates for the text within the button
        int text_x = b->x + (b->width - text_width) / 2;
        int text_y = b->y + (b->height - text_height) / 2;
        
        // Draw the text string centered on the button
        draw_string(b->font, b->text, text_x, text_y);
    }
}

bool button_is_hovered(const button_t *b, int mouse_x, int mouse_y) {
    if (b == NULL) return false;
    
    return (mouse_x >= b->x && mouse_x <= (b->x + b->width) &&
            mouse_y >= b->y && mouse_y <= (b->y + b->height));
}
