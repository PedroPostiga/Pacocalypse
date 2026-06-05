#ifndef _GAME_H_
#define _GAME_H_

#include "../map/map.h"
#include "../player/player.h"
#include "../ghost/ghost.h"
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
    uint32_t run_alive_seconds;
    uint32_t run_start_seconds;
    uint8_t run_tick_counter;
    bool run_timer_started;
    bool run_summary_available;
} game_state_t;

int game_init(game_state_t* state);
int game_restart(game_state_t* state);
void game_cleanup(game_state_t* state);
void game_handle_power_up_click(game_state_t* state, int mouse_x, int mouse_y, bool left_click);
int game_update(game_state_t* state);

#endif // _GAME_H_
