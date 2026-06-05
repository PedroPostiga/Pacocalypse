#include "resources.h"
#include "../sprite.h"
#include "../font.h"
#include "../drawings/font.xpm"

int view_resources_init(view_resources_t *resources) {
    if (!resources) return 1;

    resources->sprites = NULL;
    resources->game_font = NULL;

    resources->game_font = font_create(32, (xpm_map_t)font_xpm);
    if (resources->game_font == NULL) {
        return 1;
    }

    if (load_sprites(&resources->sprites) != 0) {
        font_destroy(resources->game_font);
        resources->game_font = NULL;
        return 1;
    }

    return 0;
}

void view_resources_cleanup(view_resources_t *resources) {
    if (!resources) return;
    destroy_sprites(&resources->sprites);
    if (resources->game_font) {
        font_destroy(resources->game_font);
        resources->game_font = NULL;
    }
}
