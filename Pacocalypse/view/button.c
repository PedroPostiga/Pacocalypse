#include "button.h"
#include "renderer/renderer.h"

void button_draw(button_t *b, bool hover) {
    // Select the appropriate sprite based on hover state
    sprite_t *curr_sp = hover ? b->hover_sp : b->sp;
    
    // Draw the sprite at the button's position
    draw_sprite(curr_sp, b->x, b->y);
    
    if (b->text != NULL && b->font != NULL) {
        // Calculate the width and height of the text based on tile size
        int text_width = strlen(b->text) * b->font->tile_size;
        int text_height = b->font->tile_size;
        
        // Calculate centered coordinates for the text within the button
        int text_x = b->x + (curr_sp->width - text_width) / 2;
        int text_y = b->y + (curr_sp->height - text_height) / 2;
        
        // Draw the text string centered on the button
        draw_string(b->font, b->text, text_x, text_y);
    }
}

bool button_is_hovered(button_t *b, int mouse_x, int mouse_y) {
    if (b == NULL || b->sp == NULL) return false;
    
    return (mouse_x >= b->x && mouse_x <= (b->x + b->sp->width) &&
            mouse_y >= b->y && mouse_y <= (b->y + b->sp->height));
}
