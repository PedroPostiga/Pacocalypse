#include <lcom/lcf.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "renderer.h"
#include "../../../lab5/videocard.h"
#include "../../model/game/game.h"
#include "../sprite.h"

static vbe_mode_info_t vmi;

static void renderer_draw_map(const map_t* map) {
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
                        int s = 4;
                        vg_draw_rectangle(x + (TILE_SIZE / 2) - (s / 2), y + (TILE_SIZE - s) / 2,
                                          s, s, 0xFFFFCC);
                    }
                    break;
                case TILE_POWER_UP:
                    if (!tile.collected) {
                        int s = 10;
                        vg_draw_rectangle(x + (TILE_SIZE / 2) - (s / 2), y + (TILE_SIZE - s) / 2,
                                          s, s, 0xFF6600);
                    }
                    break;
                default:
                    break;
            }
        }
    }
}

static void renderer_draw_player(const player_t* player) {
    if (!player || !player->alive) return;
    vg_draw_rectangle(player->x, player->y, TILE_SIZE, TILE_SIZE, 0xFFFF00);
}

static uint32_t ghost_color(const ghost_t* ghost) {
    if (ghost->state == GHOST_FRIGHTENED)
        return 0x0000CC;
    if (ghost->state == GHOST_DEAD)
        return 0x000000;
    switch (ghost->id) {
        case GHOST_RED: return 0xFF0000;
        case GHOST_PINK: return 0xFFB8FF;
        case GHOST_CYAN: return 0x00FFFF;
        case GHOST_ORANGE: return 0xFFB852;
        default: return 0xFFFFFF;
    }
}

static void renderer_draw_ghost(ghost_t* const ghosts[GHOST_COUNT]) {
    for (int i = 0; i < GHOST_COUNT; i++) {
        ghost_t *ghost = ghosts[i];
        if (!ghost || ghost->state == GHOST_DEAD) continue;
        vg_draw_rectangle(ghost->x, ghost->y, TILE_SIZE, TILE_SIZE, ghost_color(ghost));
    }
}

static void renderer_draw_mouse(int mouse_x, int mouse_y) {
    // Draw cursor using pre-rendered XPM for better performance
    sprite_draw(cursor, mouse_x, mouse_y);
}

int renderer_init(void) {
    return vbe_get_mode_info(VIDEO_MODE, &vmi) != 0;
}

void renderer_cleanup() {
    /* Nothing to free here: rendering is handled by lab5's videocard library. */
}

void renderer_draw_game(const game_state_t* state) {
    if (!state) return;

    vg_draw_rectangle(0, 0, vmi.XResolution, vmi.YResolution, 0x111111);

    switch (state->mode) {
        case STATE_MENU:
            break;
        case STATE_PLAYING:
            renderer_draw_map(state->map);
            renderer_draw_ghost(state->ghosts);
            renderer_draw_player(state->player);
            break;
        case STATE_PAUSED:
            renderer_draw_map(state->map);
            renderer_draw_ghost(state->ghosts);
            renderer_draw_player(state->player);
            vg_draw_rectangle(vmi.XResolution / 2 - 100,
                              vmi.YResolution / 2 - 30,
                              200, 60, 0x333333);
            break;
        case STATE_GAME_OVER:
            break;
        case STATE_QUIT:
            break;
    }
    
    // Draw mouse cursor on top of everything
    renderer_draw_mouse(state->mouse_x, state->mouse_y);

    vg_flip();
}
