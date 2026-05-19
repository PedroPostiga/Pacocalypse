#ifndef _GAME_H_
#define _GAME_H_
#define TICKRATE 60

#include "map/map.h"
#include "player/player.h"

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
} game_state_t;


int game_update(game_state_t* state);

int game_render(game_state_t* state);

#endif // _GAME_H_
