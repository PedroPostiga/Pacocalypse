#include "sprite.h"
#include "../../../lab5/videocard.h"
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
#include "drawings/menu_background.xpm"
#include "drawings/orange_ghost.xpm"
#include "drawings/orange_ghost_down.xpm"
#include "drawings/orange_ghost_left.xpm"
#include "drawings/orange_ghost_up.xpm"
#include "drawings/pacmanhalfclosed.xpm"
#include "drawings/pacmanhalfcloseddown.xpm"
#include "drawings/pacmanhalfclosedleft.xpm"
#include "drawings/pacmanhalfclosedup.xpm"
#include "drawings/pacmanmouthclosed.xpm"
#include "drawings/pacmanmouthopen.xpm"
#include "drawings/pacmanmouthopendown.xpm"
#include "drawings/pacmanmouthopenleft.xpm"
#include "drawings/pacmanmouthopenup.xpm"
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
#include <lcom/lcf.h>

sprite_t *sprite_create(xpm_map_t sprite) {
    sprite_t *s = malloc (sizeof(sprite_t));
    if (!s) return NULL;

    xpm_image_t img;

    uint8_t *pixmap = xpm_load(sprite, XPM_INDEXED, &img);

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

void load_sprites() {
    pebble = sprite_create((xpm_map_t)pebble_xpm);
    power_up = sprite_create((xpm_map_t)power_up_xpm);
    map = sprite_create((xpm_map_t)map_xpm);
    menu_bg = sprite_create((xpm_map_t)menu_bg_xpm);

    ghost_eyes = animated_sprite_create((xpm_map_t[]){eyes_up_xpm, eyes_right_xpm, eyes_down_xpm, eyes_left_xpm}, 4, 10, 0, 0);
    player_anim_up = animated_sprite_create((xpm_map_t[]){pacmanmouthclosed_xpm, pacmanhalfclosedup_xpm, pacmanmouthopenup_xpm}, 3, 5, 0, 0);
    player_anim_down = animated_sprite_create((xpm_map_t[]){pacmanmouthclosed_xpm, pacmanhalfcloseddown_xpm, pacmanmouthopendown_xpm}, 3, 5, 0, 0);
    player_anim_left = animated_sprite_create((xpm_map_t[]){pacmanmouthclosed_xpm, pacmanhalfclosedleft_xpm, pacmanmouthopenleft_xpm}, 3, 5, 0, 0);
    player_anim_right = animated_sprite_create((xpm_map_t[]){pacmanmouthclosed_xpm, pacmanhalfclosedright_xpm, pacmanmouthopenright_xpm}, 3, 5, 0, 0);
    ghost_cyan = animated_sprite_create((xpm_map_t[]){cyan_ghost_up_xpm, cyan_ghost_right_xpm, cyan_ghost_down_xpm, cyan_ghost_left_xpm}, 4, 10, 0, 0);
    ghost_red = animated_sprite_create((xpm_map_t[]){red_ghost_up_xpm, red_ghost_right_xpm, red_ghost_down_xpm, red_ghost_left_xpm}, 4, 10, 0, 0);
    ghost_pink = animated_sprite_create((xpm_map_t[]){pink_ghost_up_xpm, pink_ghost_right_xpm, pink_ghost_down_xpm, pink_ghost_left_xpm}, 4, 10, 0, 0);
    ghost_orange = animated_sprite_create((xpm_map_t[]){orange_ghost_up_xpm, orange_ghost_right_xpm, orange_ghost_down_xpm, orange_ghost_left_xpm}, 4, 10, 0, 0);
    frightened_ghost_anim = animated_sprite_create((xpm_map_t[]){ghost_scared_blue_xpm, ghost_scared_white_xpm}, 2, 10, 0, 0);
}

void destroy_sprites() {
    sprite_destroy(pebble);
    sprite_destroy(power_up);
    sprite_destroy(map);
    sprite_destroy(menu_bg);

    animated_sprite_destroy(ghost_eyes);
    animated_sprite_destroy(player_anim_up);
    animated_sprite_destroy(player_anim_down);
    animated_sprite_destroy(player_anim_left);
    animated_sprite_destroy(player_anim_right);
    animated_sprite_destroy(ghost_cyan);
    animated_sprite_destroy(ghost_red);
    animated_sprite_destroy(ghost_pink);
    animated_sprite_destroy(ghost_orange);
    animated_sprite_destroy(frightened_ghost_anim);
}

int draw_sprite(const sprite_t* sprite, int x, int y) {
    if (!sprite) return -1;
    
    uint8_t *color = sprite->pixmap;

    for (int j = 0; j < sprite->height; j++) {
        for (int i = 0; i < sprite->width; i++) {
            uint8_t pixel = color[j * sprite->width + i];
            if (pixel != 0) { // Assuming 0 is transparent
                vg_draw_pixel(x + i, y + j, pixel);
            }
        }
    }
    return 0;
}

int draw_animated_sprite(const animated_sprite_t* sprite) {
    if (!sprite) return -1;
    return draw_sprite(sprite->frames[sprite->current_pixmap], sprite->x, sprite->y);
}
