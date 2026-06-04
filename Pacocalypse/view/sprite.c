#include "sprite.h"
#include "drawings/cyan_ghost.xpm"
#include "drawings/cyan_ghost_down.xpm"
#include "drawings/cyan_ghost_left.xpm"
#include "drawings/cyan_ghost_up.xpm"
#include "drawings/eyes_down.xpm"
#include "drawings/eyes_left.xpm"
#include "drawings/eyes_right.xpm"
#include "drawings/eyes_up.xpm"
#include "drawings/ghost_scared_blue.xpm"
#include "drawings/ghost_scared_white.xpm"
#include "drawings/map.xpm"
#include "drawings/orange_ghost.xpm"
#include "drawings/orange_ghost_down.xpm"
#include "drawings/orange_ghost_left.xpm"
#include "drawings/orange_ghost_up.xpm"
#include "drawings/pacmanhalfclosed_right.xpm"
#include "drawings/pacmanhalfclosed_down.xpm"
#include "drawings/pacmanhalfclosed_left.xpm"
#include "drawings/pacmanhalfclosed_up.xpm"
#include "drawings/pacmanmouthclosed.xpm"
#include "drawings/pacmanmouthopen_right.xpm"
#include "drawings/pacmanmouthopen_down.xpm"
#include "drawings/pacmanmouthopen_left.xpm"
#include "drawings/pacmanmouthopen_up.xpm"
#include "drawings/pebble.xpm"
#include "drawings/pink_ghost.xpm"
#include "drawings/pink_ghost_down.xpm"
#include "drawings/pink_ghost_left.xpm"
#include "drawings/pink_ghost_up.xpm"
#include "drawings/power_up.xpm"
#include "drawings/red_ghost.xpm"
#include "drawings/red_ghost_down.xpm"
#include "drawings/red_ghost_left.xpm"
#include "drawings/red_ghost_up.xpm"
#include "drawings/menu_background.xpm"
#include "drawings/cursor.xpm"

#include <lcom/lcf.h>
#include <stdio.h>
#include <string.h>

sprite_t *sprite_create(xpm_map_t sprite) {
    sprite_t *s = malloc (sizeof(sprite_t));
    if (!s) return NULL;

    xpm_image_t img;

    uint8_t *pixmap = xpm_load(sprite, XPM_8_8_8_8, &img);

    if (!pixmap) {
        free(s);
        return NULL;
    }

    s->pixmap = pixmap;
    s->width = img.width;
    s->height = img.height;
    
    return s;
}

void sprite_destroy(sprite_t* sprite) {
    if (!sprite) return;
    free(sprite->pixmap);
    free(sprite);
}

animated_sprite_t *animated_sprite_create(xpm_map_t xpms[], uint8_t num_frames, uint8_t ticks_per_frame, uint16_t x, uint16_t y) {
    animated_sprite_t *anim = malloc(sizeof(animated_sprite_t));
    if (!anim) return NULL;

    anim->frames = malloc(num_frames * sizeof(sprite_t*));
    if (!anim->frames) {
        free(anim);
        return NULL;
    }

    for (int i = 0; i < num_frames; i++) {
        anim->frames[i] = sprite_create(xpms[i]);
        if (!anim->frames[i]) {
            // Clean up previously created frames
            for (int j = 0; j < i; j++) {
                sprite_destroy(anim->frames[j]);
            }
            free(anim->frames);
            free(anim);
            return NULL;
        }
    }

    anim->no_pixmaps = num_frames;
    anim->current_pixmap = 0;
    anim->ticks_per_frame = ticks_per_frame;
    anim->tick_count = 0;
    anim->x = x;
    anim->y = y;

    return anim;
}

void animated_sprite_destroy(animated_sprite_t* sprite) {
    if (!sprite) return;
    for (int i = 0; i < sprite->no_pixmaps; i++) {
        sprite_destroy(sprite->frames[i]);
    }
    free(sprite->frames);
    free(sprite);
}

void animated_sprite_update(animated_sprite_t* sprite) {
    sprite->tick_count++;
    if (sprite->tick_count >= sprite->ticks_per_frame) {
        sprite->tick_count = 0;
        sprite->current_pixmap = (sprite->current_pixmap + 1) % sprite->no_pixmaps;
    }
}

sprite_t* animated_sprite_get_current_frame(animated_sprite_t* sprite) {
    return sprite->frames[sprite->current_pixmap];
}

int load_sprites(game_sprites_t **sprites) {
    if (!sprites) return 1;

    game_sprites_t *loaded = malloc(sizeof(game_sprites_t));
    if (!loaded) return 1;

    // Zero-initialize the struct safely
    memset(loaded, 0, sizeof(*loaded));

    loaded->pebble = sprite_create((xpm_map_t)pebble_xpm);
    loaded->power_up = sprite_create((xpm_map_t)power_up_xpm);
    loaded->map = sprite_create((xpm_map_t)map_xpm);
    loaded->menu_bg = sprite_create((xpm_map_t)menu_background_xpm);
    loaded->cursor = sprite_create((xpm_map_t)cursor_xpm);

    if (!loaded->pebble || !loaded->power_up || !loaded->map || !loaded->menu_bg || !loaded->cursor) {
        destroy_sprites(&loaded);
        return 1;
    }

    xpm_map_t ghost_eyes_xpms[] = { (xpm_map_t)eyes_up_xpm, (xpm_map_t)eyes_right_xpm, (xpm_map_t)eyes_down_xpm, (xpm_map_t)eyes_left_xpm };
    loaded->ghost_eyes = animated_sprite_create(ghost_eyes_xpms, 4, 10, 0, 0);

    xpm_map_t player_up_xpms[] = { (xpm_map_t)pacmanmouthclosed_xpm, (xpm_map_t)pacmanhalfclosed_up_xpm, (xpm_map_t)pacmanmouthopen_up_xpm };
    loaded->player_anim_up = animated_sprite_create(player_up_xpms, 3, 5, 0, 0);

    xpm_map_t player_down_xpms[] = { (xpm_map_t)pacmanmouthclosed_xpm, (xpm_map_t)pacmanhalfclosed_down_xpm, (xpm_map_t)pacmanmouthopen_down_xpm };
    loaded->player_anim_down = animated_sprite_create(player_down_xpms, 3, 5, 0, 0);

    xpm_map_t player_left_xpms[] = { (xpm_map_t)pacmanmouthclosed_xpm, (xpm_map_t)pacmanhalfclosed_left_xpm, (xpm_map_t)pacmanmouthopen_left_xpm };
    loaded->player_anim_left = animated_sprite_create(player_left_xpms, 3, 5, 0, 0);

    xpm_map_t player_right_xpms[] = { (xpm_map_t)pacmanmouthclosed_xpm, (xpm_map_t)pacmanhalfclosed_right_xpm, (xpm_map_t)pacmanmouthopen_right_xpm };
    loaded->player_anim_right = animated_sprite_create(player_right_xpms, 3, 5, 0, 0);

    xpm_map_t ghost_cyan_xpms[] = { (xpm_map_t)cyan_ghost_up_xpm, (xpm_map_t)cyan_ghost_xpm, (xpm_map_t)cyan_ghost_down_xpm, (xpm_map_t)cyan_ghost_left_xpm };
    loaded->ghost_cyan = animated_sprite_create(ghost_cyan_xpms, 4, 10, 0, 0);

    xpm_map_t ghost_red_xpms[] = { (xpm_map_t)red_ghost_up_xpm, (xpm_map_t)red_ghost_xpm, (xpm_map_t)red_ghost_down_xpm, (xpm_map_t)red_ghost_left_xpm };
    loaded->ghost_red = animated_sprite_create(ghost_red_xpms, 4, 10, 0, 0);

    xpm_map_t ghost_pink_xpms[] = { (xpm_map_t)pink_ghost_up_xpm, (xpm_map_t)pink_ghost_xpm, (xpm_map_t)pink_ghost_down_xpm, (xpm_map_t)pink_ghost_left_xpm };
    loaded->ghost_pink = animated_sprite_create(ghost_pink_xpms, 4, 10, 0, 0);

    xpm_map_t ghost_orange_xpms[] = { (xpm_map_t)orange_ghost_up_xpm, (xpm_map_t)orange_ghost_xpm, (xpm_map_t)orange_ghost_down_xpm, (xpm_map_t)orange_ghost_left_xpm };
    loaded->ghost_orange = animated_sprite_create(ghost_orange_xpms, 4, 10, 0, 0);

    xpm_map_t frightened_xpms[] = { (xpm_map_t)ghost_scared_blue_xpm, (xpm_map_t)ghost_scared_white_xpm };
    loaded->frightened_ghost_anim = animated_sprite_create(frightened_xpms, 2, 10, 0, 0);

    if (!loaded->ghost_eyes || !loaded->player_anim_up || !loaded->player_anim_down || !loaded->player_anim_left || !loaded->player_anim_right ||
        !loaded->ghost_cyan || !loaded->ghost_red || !loaded->ghost_pink || !loaded->ghost_orange || !loaded->frightened_ghost_anim) {
        destroy_sprites(&loaded);
        return 1;
    }

    *sprites = loaded;
    return 0;
}

void destroy_sprites(game_sprites_t **sprites) {
    if (!sprites || !*sprites) return;

    game_sprites_t *s = *sprites;

    sprite_destroy(s->pebble);
    sprite_destroy(s->power_up);
    sprite_destroy(s->map);
    sprite_destroy(s->menu_bg);
    sprite_destroy(s->cursor);

    animated_sprite_destroy(s->ghost_eyes);
    animated_sprite_destroy(s->player_anim_up);
    animated_sprite_destroy(s->player_anim_down);
    animated_sprite_destroy(s->player_anim_left);
    animated_sprite_destroy(s->player_anim_right);
    animated_sprite_destroy(s->ghost_cyan);
    animated_sprite_destroy(s->ghost_red);
    animated_sprite_destroy(s->ghost_pink);
    animated_sprite_destroy(s->ghost_orange);
    animated_sprite_destroy(s->frightened_ghost_anim);

    free(s);
    *sprites = NULL;
}
