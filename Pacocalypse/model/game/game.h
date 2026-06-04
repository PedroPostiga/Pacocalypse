#ifndef _GAME_H_
#define _GAME_H_
#define TICKRATE 60
#define TICKS_PER_UPDATE 1
#define VIDEO_MODE 0x14C
#define CURSOR_SIZE 10

#include "../map/map.h"
#include "../player/player.h"
#include "../ghost/ghost.h"
#include "../../../lab4/mouse.h"
#include "../../view/sprite.h"
#include <stdlib.h>

typedef enum {
    STATE_MENU,
    STATE_PLAYING,
    STATE_PAUSED,
    STATE_GAME_OVER,
    STATE_QUIT
} game_mode_t;

typedef struct {
    game_mode_t mode;
    player_t *player;
    map_t *map;
    ghost_t *ghosts[GHOST_COUNT]; // Assuming a maximum of 4 ghosts
    int num_ghosts;
    int mouse_x;
    int mouse_y;
    game_sprites_t *sprites;
} game_state_t;

int game_init(game_state_t* state);
void game_cleanup(game_state_t* state);
void game_handle_mouse(game_state_t* state, struct packet* mouse_packet);
int game_update(game_state_t* state);


#endif // _GAME_H_
