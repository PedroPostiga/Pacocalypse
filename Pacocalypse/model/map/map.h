#ifndef _MAP_H_
#define _MAP_H_

#include <stdint.h>
#include <stdbool.h>

#define SCREEN_WIDTH 1152
#define SCREEN_HEIGHT 864
#define MAP_ROWS 21
#define MAP_COLS 21
#define TILE_SIZE 32
#define MAP_OFFSET_X (((SCREEN_WIDTH - (MAP_COLS * TILE_SIZE)) / 2)) // = 176
#define MAP_OFFSET_Y (((SCREEN_HEIGHT - (MAP_ROWS * TILE_SIZE)) / 2)) // = 48
#define MAP_TUNNEL_ROW 11

typedef enum {
    TILE_EMPTY,
    TILE_WALL,
    TILE_PELLET,
    TILE_POWER_UP,
    TILE_GHOST_SPAWN,
    TILE_PLAYER_SPAWN
} tile_type_t;

typedef struct {
    tile_type_t type;
    bool collected;  // For pellets and power-ups
} tile_t;

typedef struct {
    tile_t tiles[MAP_ROWS][MAP_COLS];
    uint16_t pellets_remaining;
} map_t;

//lifecycle
map_t *map_create();
void map_destroy(map_t *map);

//queries
tile_type_t map_get_tile(const map_t *map, int row, int col);
bool map_is_walkable(const map_t *map, int row, int col);
void map_tile_from_pixel(int x, int y, int *row, int *col);

//mutations
void map_collect_pellet(map_t *map, int row, int col);
bool map_all_pellets_collected(const map_t *map);

#endif // _MAP_H_
