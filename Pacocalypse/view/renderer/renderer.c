#include <lcom/lcf.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "renderer.h"
#include "../../../lab5/videocard.h"
#include "../../model/game/game.h"
#include "../../model/map/map.h"
#include "../../model/input/input.h"
#include "../hud.h"
#include "../sprite.h"
#include "../resources/resources.h"
#include "../ui/menu.h"
#include "../ui/pause.h"

static vbe_mode_info_t vmi;

static uint8_t direction_to_ghost_frame(direction_t dir) {
    switch (dir) {
        case DIR_UP:    return 0;
        case DIR_RIGHT: return 1;
        case DIR_DOWN:  return 2;
        case DIR_LEFT:  return 3;
        default:        return 1;
    }
}

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
    
    // Show eyes when dead or respawning
    if (ghost->state == GHOST_DEAD || ghost->state == GHOST_RESPAWNING)
        return sprites->ghost_eyes;
    
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

// Maps ghost direction to eyes sprite frame index
// Eyes frames are ordered: [UP=0, RIGHT=1, DOWN=2, LEFT=3]
static int renderer_get_eyes_frame(direction_t direction) {
    switch (direction) {
        case DIR_UP:    return 0;  // eyes_up
        case DIR_DOWN:  return 2;  // eyes_down
        case DIR_LEFT:  return 3;  // eyes_left
        case DIR_RIGHT: return 1;  // eyes_right
        default:        return 0;  // Default to up
    }
}

static void renderer_draw_ghost(ghost_t* const ghosts[GHOST_COUNT], const game_sprites_t *sprites) {
    for (int i = 0; i < GHOST_COUNT; i++) {
        ghost_t *ghost = ghosts[i];
        if (!ghost) continue;
        
        // Skip drawing if ghost is permanently dead (not respawning)
        if (ghost->state == GHOST_DEAD) {
            // Still need to draw eyes
        }

        animated_sprite_t *ghost_anim = renderer_select_ghost_sprite(ghost, sprites);
        if (!ghost_anim) continue;

        // For eyes sprites, set the frame based on direction
        if (ghost->state == GHOST_DEAD || ghost->state == GHOST_RESPAWNING) {
            int eyes_frame = renderer_get_eyes_frame(ghost->direction);
            if (eyes_frame >= 0 && eyes_frame < (int)ghost_anim->no_pixmaps) {
                ghost_anim->current_pixmap = eyes_frame;
            }
        }

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

void renderer_update_animations(const game_state_t *state, const view_resources_t *resources) {
    if (!state || !resources || !resources->sprites) return;

    game_sprites_t *sp = resources->sprites;

    if (sp->player_anim_up)    animated_sprite_update(sp->player_anim_up);
    if (sp->player_anim_down)  animated_sprite_update(sp->player_anim_down);
    if (sp->player_anim_left)  animated_sprite_update(sp->player_anim_left);
    if (sp->player_anim_right) animated_sprite_update(sp->player_anim_right);

    animated_sprite_t *ghost_sprites[GHOST_COUNT] = {
        sp->ghost_red,
        sp->ghost_pink,
        sp->ghost_cyan,
        sp->ghost_orange
    };

    for (int i = 0; i < state->num_ghosts; i++) {
        ghost_t *ghost = state->ghosts[i];
        if (!ghost) continue;

        animated_sprite_t *anim = ghost_sprites[ghost->id];
        if (anim) {
            anim->current_pixmap = direction_to_ghost_frame(ghost->direction);
        }
    }

    if (sp->frightened_ghost_anim) animated_sprite_update(sp->frightened_ghost_anim);
    if (sp->ghost_eyes)            animated_sprite_update(sp->ghost_eyes);
}

void renderer_draw_game(const game_state_t* state,
                        const view_resources_t *resources,
                        const input_state_t *input,
                        const menu_state_t *menu,
                        const pause_state_t *pause) {
    if (!state || !resources || !resources->sprites || !input || !menu || !pause) return;

    vg_draw_rectangle(0, 0, vmi.XResolution, vmi.YResolution, 0x111111);

    switch (state->mode) {
        case STATE_MENU:
            menu_draw(menu, input, resources->sprites);
            break;
        case STATE_PLAYING:
            renderer_draw_map(state->map, resources->sprites);
            renderer_draw_ghost(state->ghosts, resources->sprites);
            renderer_draw_player(state->player, resources->sprites);
            hud_draw(state->player, resources->game_font, state->alive_seconds);
            break;
        case STATE_PAUSED:
            renderer_draw_map(state->map, resources->sprites);
            renderer_draw_ghost(state->ghosts, resources->sprites);
            renderer_draw_player(state->player, resources->sprites);
            hud_draw(state->player, resources->game_font, state->alive_seconds);
            pause_draw(pause, input, resources->sprites);
            break;
        case STATE_GAME_OVER:
            hud_draw(state->player, resources->game_font, state->alive_seconds);
            draw_string(resources->game_font, "GAME OVER", SCREEN_WIDTH / 2 - 72, SCREEN_HEIGHT / 2 - 16);
            draw_string(resources->game_font, "ESC MENU", SCREEN_WIDTH / 2 - 64, SCREEN_HEIGHT / 2 + 24);
            break;
        case STATE_QUIT:
            break;
    }
    
    renderer_draw_mouse(input->mouse_x, input->mouse_y, resources->sprites->cursor);

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
