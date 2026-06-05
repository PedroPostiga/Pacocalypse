#include "map.h"
#include <stdlib.h>
#include <string.h>

// W = wall, . = pellet, o = power-up, P = player spawn, G = ghost spawn, space = empty
static const char *default_map_layout[MAP_ROWS] = {
    "WWWWWWWWWWWWWWWWWWWWW",
    "W.........W.........W",
    "W.WWW.WWW.W.WWW.WWWoW",
    "W.WWW.WWW.W.WWW.WWW.W",
    "W...................W",
    "W.WWW.W...W...W.WWW.W",
    "W.WWW.WWW.W.WWW.WWW.W",
    "W...o.W...W...W.....W",
    "WWWW.............WWWW",
    "WWWW....WW WW....WWWW",
    "WWWW..W.W G W.W..WWWW",
    "......W.WGGGW.W......",
    "WWWW..W.WWWWW.W..WWWW",
    "WWWW..W.......W..WWWW",
    "WWWW.WW.WWWWW.WW.WWWW",
    "W.........W......o..W",
    "W..WW.WW..W..WW.WW..W",
    "WW..W.W...P...W.W..WW",
    "W.....W..WWW..W.....W",
    "W.......o...........W",
    "WWWWWWWWWWWWWWWWWWWWW"
};

map_t *map_create() {
    map_t *map = malloc(sizeof(map_t));
    if (!map) return NULL;

    memset(map, 0, sizeof(map_t)); // Initialize all fields to zero

    map->pellets_remaining = TOTAL_PELLETS;

    for (int row = 0; row <MAP_ROWS; row++) {
        for(int col = 0; col <MAP_COLS; col++) {
            tile_t *tile = &map->tiles[row][col];
            tile->collected = false;

            switch(default_map_layout[row][col]) {
                case 'W':
                    tile->type = TILE_WALL;
                    break;
                case '.':
                    tile->type = TILE_PELLET;
                    break;
                case 'o':
                    tile->type = TILE_POWER_UP;
                    break;
                case 'P':
                    tile->type = TILE_PLAYER_SPAWN;
                    break;
                case 'G':
                    tile->type = TILE_GHOST_SPAWN;
                    break;
                default:
                    tile->type = TILE_EMPTY;
            }
        }
    }
    return map;
}

void map_destroy(map_t *map) {
    free(map);
}

tile_type_t map_get_tile(const map_t *map, int row, int col) {
    if (row < 0 || row >= MAP_ROWS || col < 0 || col >= MAP_COLS)
        return TILE_WALL; // Treat out-of-bounds as walls
    return map->tiles[row][col].type;
}

bool map_is_walkable(const map_t *map, int row, int col) {
    tile_type_t type = map_get_tile(map, row, col);
    return type != TILE_WALL;
}

void map_collect_pellet(map_t *map, int row, int col) {
    if (row < 0 || row >= MAP_ROWS || col < 0 || col >= MAP_COLS)
        return;

    tile_t *tile = &map->tiles[row][col];
    if ((tile->type == TILE_PELLET || tile->type == TILE_POWER_UP) && !tile->collected) {
        tile->collected = true;
        if (tile->type == TILE_PELLET)
            map->pellets_remaining--;
    }
}

bool map_all_pellets_collected(const map_t *map) {
    return map->pellets_remaining == 0;
}

void map_tile_from_pixel(int x, int y, int *row, int *col) {
    if (x < MAP_OFFSET_X || y < MAP_OFFSET_Y) {
        *row = -1;
        *col = -1;
        return;
    }
    *col = (x - MAP_OFFSET_X) / TILE_SIZE;
    *row = (y - MAP_OFFSET_Y) / TILE_SIZE;
}
