#include <lcom/lcf.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "renderer.h"
#include "../../../lab5/videocard.h"
#include "../../model/game/game.h"
#include "../../model/map/map.h"
#include "../hud.h"
#include "../sprite.h"

static vbe_mode_info_t vmi;

static void renderer_draw_tile_sprite(const sprite_t* sprite, int x, int y) {
    if (!sprite) return;

    draw_sprite(sprite,
                x + (TILE_SIZE - sprite->width) / 2,
                y + (TILE_SIZE - sprite->height) / 2);
}

static void renderer_draw_map(const map_t* map, const game_sprites_t *sprites) {
    for (int row = 0; row < MAP_ROWS; row++) {
        for (int col = 0; col < MAP_COLS; col++) {
            tile_t tile = map->tiles[row][col];
            int x = MAP_OFFSET_X + col * TILE_SIZE;
            int y = MAP_OFFSET_Y + row * TILE_SIZE;

            switch (tile.type) {
                case TILE_WALL:
                    vg_draw_rectangle(x, y, TILE_SIZE, TILE_SIZE, 0x1A1AFF);
                    break;
                case TILE_PELLET:
                    if (!tile.collected) {
                        renderer_draw_tile_sprite(sprites->pebble, x, y);
                    }
                    break;
                case TILE_POWER_UP:
                    if (!tile.collected) {
                        renderer_draw_tile_sprite(sprites->power_up, x, y);
                    }
                    break;
                default:
                    break;
            }
        }
    }
}

static void renderer_draw_player(const player_t* player, const game_sprites_t *sprites) {
    if (!player || !player->alive || !sprites) return;

    animated_sprite_t *player_anim = NULL;
    switch (player->direction) {
        case DIR_UP: player_anim = sprites->player_anim_up; break;
        case DIR_DOWN: player_anim = sprites->player_anim_down; break;
        case DIR_LEFT: player_anim = sprites->player_anim_left; break;
        case DIR_RIGHT: player_anim = sprites->player_anim_right; break;
        default: player_anim = sprites->player_anim_right; break;
    }

    if (!player_anim) return;
    sprite_t *frame = animated_sprite_get_current_frame(player_anim);
    draw_sprite(frame, player->x, player->y);
}

static animated_sprite_t* renderer_select_ghost_sprite(const ghost_t* ghost, const game_sprites_t *sprites) {
    if (!ghost || !sprites) return NULL;
    if (ghost->state == GHOST_FRIGHTENED)
        return sprites->frightened_ghost_anim;

    switch (ghost->id) {
        case GHOST_RED: return sprites->ghost_red;
        case GHOST_PINK: return sprites->ghost_pink;
        case GHOST_CYAN: return sprites->ghost_cyan;
        case GHOST_ORANGE: return sprites->ghost_orange;
        default: return NULL;
    }
}

static void renderer_draw_ghost(ghost_t* const ghosts[GHOST_COUNT], const game_sprites_t *sprites) {
    for (int i = 0; i < GHOST_COUNT; i++) {
        ghost_t *ghost = ghosts[i];
        if (!ghost || ghost->state == GHOST_DEAD) continue;

        animated_sprite_t *ghost_anim = renderer_select_ghost_sprite(ghost, sprites);
        if (!ghost_anim) continue;

        sprite_t *frame = animated_sprite_get_current_frame(ghost_anim);
        draw_sprite(frame, ghost->x, ghost->y);
    }
}

static void renderer_draw_mouse(int mouse_x, int mouse_y, sprite_t* cursor_sprite) {
    // Draw cursor using pre-rendered XPM for better performance
    draw_sprite(cursor_sprite, mouse_x, mouse_y);
}

int renderer_init(void) {
    return vbe_get_mode_info(VIDEO_MODE, &vmi) != 0;
}

int renderer_cleanup() {
    /* Nothing to free here: rendering is handled by lab5's videocard library. */
    return 0;
}

void renderer_draw_button(const button_t *btn, int mouse_x, int mouse_y) {
    if (!btn) return;
    bool is_hovered = button_is_hovered(btn, mouse_x, mouse_y);
    button_draw(btn, is_hovered);
}

void renderer_draw_game(const game_state_t* state) {
    if (!state || !state->sprites) return;

    vg_draw_rectangle(0, 0, vmi.XResolution, vmi.YResolution, 0x111111);

    switch (state->mode) {
        case STATE_MENU:
            if (state->sprites->menu_bg) {
                int bg_x = (vmi.XResolution - state->sprites->menu_bg->width) / 2;
                int bg_y = (vmi.YResolution - state->sprites->menu_bg->height) / 2;
                if (bg_x < 0) bg_x = 0;
                if (bg_y < 0) bg_y = 0;
                draw_sprite(state->sprites->menu_bg, bg_x, bg_y);
            }
            renderer_draw_button(&state->play_button, state->mouse_x, state->mouse_y);
            renderer_draw_button(&state->quit_button, state->mouse_x, state->mouse_y);
            break;
        case STATE_PLAYING:
            renderer_draw_map(state->map, state->sprites);
            renderer_draw_ghost(state->ghosts, state->sprites);
            renderer_draw_player(state->player, state->sprites);
            hud_draw(state->player, state->game_font);
            break;
        case STATE_PAUSED:
            renderer_draw_map(state->map, state->sprites);
            renderer_draw_ghost(state->ghosts, state->sprites);
            renderer_draw_player(state->player, state->sprites);
            hud_draw(state->player, state->game_font);
            renderer_draw_button(&state->paused_button, state->mouse_x, state->mouse_y);
            break;
        case STATE_GAME_OVER:
            break;
        case STATE_QUIT:
            break;
    }
    
    // Draw mouse cursor on top of everything
    renderer_draw_mouse(state->mouse_x, state->mouse_y, state->sprites->cursor);

    vg_flip();
}


int draw_sprite(const sprite_t* sprite, int x, int y) {
    if (!sprite) return -1;
    
    uint32_t *color = (uint32_t *) sprite->pixmap;
    uint32_t transparent = xpm_transparency_color(XPM_8_8_8_8);

    for (int j = 0; j < sprite->height; j++) {
        for (int i = 0; i < sprite->width; i++) {
            uint32_t pixel = color[j * sprite->width + i];
            if (pixel != transparent) {
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
