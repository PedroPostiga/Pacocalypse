#ifndef _VIEW_RESOURCES_H_
#define _VIEW_RESOURCES_H_

#include "../sprite.h"
#include "../font.h"

typedef struct {
    game_sprites_t *sprites;
    font_t *game_font;
} view_resources_t;

int view_resources_init(view_resources_t *resources);
void view_resources_cleanup(view_resources_t *resources);

#endif // _VIEW_RESOURCES_H_
