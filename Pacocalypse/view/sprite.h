#ifndef _SPRITE_H_
#define _SPRITE_H_

#include <stdint.h>
#include <stdlib.h>


typedef struct {
    uint8_t *pixmap;
    uint16_t width, height;
} sprite_t;

typedef struct {
    uint8_t no_pixmaps;
    uint8_t current_pixmap;
    sprite_t **frames; // array of pointers to sprite_t
    uint16_t x, y; // position on screen
    uint8_t ticks_per_frame; // how many game ticks to show each frame
    uint8_t tick_count; // counts ticks to determine when to switch frames
} animated_sprite_t;

sprite_t *sprite_create(xpm_map_t sprite);

void sprite_destroy(sprite_t* sprite);

animated_sprite_t *animated_sprite_create(xpm_map_t xpms[], uint8_t num_frames,uint8_t ticks_per_frame, uint16_t x, uint16_t y);

void animated_sprite_destroy(animated_sprite_t* sprite);

void animated_sprite_update(animated_sprite_t* sprite);

sprite_t* animated_sprite_get_current_frame(animated_sprite_t* sprite);

void load_sprites();

void destroy_sprites();

int draw_sprite(const sprite_t* sprite, int x, int y);

int draw_animated_sprite(const animated_sprite_t* sprite);

sprite_t* pebble;
sprite_t* power_up;
sprite_t* map;
sprite_t* menu_bg;
sprite_t* cursor;

animated_sprite_t* ghost_eyes;
animated_sprite_t* player_anim_up;
animated_sprite_t* player_anim_down;
animated_sprite_t* player_anim_left;
animated_sprite_t* player_anim_right;
animated_sprite_t* ghost_cyan;
animated_sprite_t* ghost_red;
animated_sprite_t* ghost_pink;
animated_sprite_t* ghost_orange;
animated_sprite_t* frightened_ghost_anim;


#endif // _SPRITE_H_
