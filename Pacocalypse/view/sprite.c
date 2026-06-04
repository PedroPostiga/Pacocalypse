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

game_sprites_t load_sprites() {
    game_sprites_t game_sprites;

    game_sprites.pebble = sprite_create((xpm_map_t)pebble_xpm);
    game_sprites.power_up = sprite_create((xpm_map_t)power_up_xpm);
    game_sprites.map = sprite_create((xpm_map_t)map_xpm);
    game_sprites.menu_bg = sprite_create((xpm_map_t)menu_background_xpm);
    game_sprites.cursor = sprite_create((xpm_map_t)cursor_xpm);

    game_sprites.ghost_eyes = animated_sprite_create((xpm_map_t[]){eyes_up_xpm, eyes_right_xpm, eyes_down_xpm, eyes_left_xpm}, 4, 10, 0, 0);
    game_sprites.player_anim_up = animated_sprite_create((xpm_map_t[]){pacmanmouthclosed_xpm, pacmanhalfclosed_up_xpm, pacmanmouthopen_up_xpm}, 3, 5, 0, 0);
    game_sprites.player_anim_down = animated_sprite_create((xpm_map_t[]){pacmanmouthclosed_xpm, pacmanhalfclosed_down_xpm, pacmanmouthopen_down_xpm}, 3, 5, 0, 0);
    game_sprites.player_anim_left = animated_sprite_create((xpm_map_t[]){pacmanmouthclosed_xpm, pacmanhalfclosed_left_xpm, pacmanmouthopen_left_xpm}, 3, 5, 0, 0);
    game_sprites.player_anim_right = animated_sprite_create((xpm_map_t[]){pacmanmouthclosed_xpm, pacmanhalfclosed_right_xpm, pacmanmouthopen_right_xpm}, 3, 5, 0, 0);
    game_sprites.ghost_cyan = animated_sprite_create((xpm_map_t[]){cyan_ghost_up_xpm, cyan_ghost_xpm, cyan_ghost_down_xpm, cyan_ghost_left_xpm}, 4, 10, 0, 0);
    game_sprites.ghost_red = animated_sprite_create((xpm_map_t[]){red_ghost_up_xpm, red_ghost_xpm, red_ghost_down_xpm, red_ghost_left_xpm}, 4, 10, 0, 0);
    game_sprites.ghost_pink = animated_sprite_create((xpm_map_t[]){pink_ghost_up_xpm, pink_ghost_xpm, pink_ghost_down_xpm, pink_ghost_left_xpm}, 4, 10, 0, 0);
    game_sprites.ghost_orange = animated_sprite_create((xpm_map_t[]){orange_ghost_up_xpm, orange_ghost_xpm, orange_ghost_down_xpm, orange_ghost_left_xpm}, 4, 10, 0, 0);
    game_sprites.frightened_ghost_anim = animated_sprite_create((xpm_map_t[]){ghost_scared_blue_xpm, ghost_scared_white_xpm}, 2, 10, 0, 0);
}

void destroy_sprites(game_sprites_t game_sprites) {
    sprite_destroy(game_sprites.pebble);
    sprite_destroy(game_sprites.power_up);
    sprite_destroy(game_sprites.map);
    sprite_destroy(game_sprites.menu_bg);

    animated_sprite_destroy(game_sprites.ghost_eyes);
    animated_sprite_destroy(game_sprites.player_anim_up);
    animated_sprite_destroy(game_sprites.player_anim_down);
    animated_sprite_destroy(game_sprites.player_anim_left);
    animated_sprite_destroy(game_sprites.player_anim_right);
    animated_sprite_destroy(game_sprites.ghost_cyan);
    animated_sprite_destroy(game_sprites.ghost_red);
    animated_sprite_destroy(game_sprites.ghost_pink);
    animated_sprite_destroy(game_sprites.ghost_orange);
    animated_sprite_destroy(game_sprites.frightened_ghost_anim);
}
