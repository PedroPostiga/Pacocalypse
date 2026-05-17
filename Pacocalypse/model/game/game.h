#ifndef _GAME_H_
#define _GAME_H_

typedef enum {
    STATE_MENU,
    STATE_PLAYING,
    STATE_PAUSED,
    STATE_GAME_OVER,
    STATE_QUIT
} game_mode_t;

typedef struct {
    game_mode_t mode;
}

#endif // _GAME_H_
