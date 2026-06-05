#ifndef _GAME_H_
#define _GAME_H_
#define TICKRATE 60
#define TICKS_PER_UPDATE 1
#define VIDEO_MODE 0x14C
#define CURSOR_SIZE 10

#include "../map/map.h"
#include "../player/player.h"
#include "../ghost/ghost.h"
#include "../input/input.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

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
    ghost_t *ghosts[GHOST_COUNT];
    int num_ghosts;
    uint32_t alive_seconds;
    uint32_t rtc_start_seconds;
    uint8_t alive_tick_counter;
    bool rtc_timer_started;
} game_state_t;

int game_init(game_state_t* state);
int game_restart(game_state_t* state);
void game_cleanup(game_state_t* state);
void game_handle_mouse_click(game_state_t* state, const input_state_t *input);
int game_update(game_state_t* state);

#endif // _GAME_H_
