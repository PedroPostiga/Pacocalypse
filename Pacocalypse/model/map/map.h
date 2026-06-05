#ifndef _MAP_H_
#define _MAP_H_

#include <stdint.h>
#include <stdbool.h>
#include "../../config.h"

/** @brief Number of rows in the tile map. */
#define MAP_ROWS 21

/** @brief Number of columns in the tile map. */
#define MAP_COLS 21

/** @brief Size in pixels of one square map tile. */
#define TILE_SIZE 32

/** @brief Horizontal pixel offset used to center the map on screen. */
#define MAP_OFFSET_X (((SCREEN_WIDTH - (MAP_COLS * TILE_SIZE)) / 2)) // = 176

/** @brief Vertical pixel offset used to center the map on screen. */
#define MAP_OFFSET_Y (((SCREEN_HEIGHT - (MAP_ROWS * TILE_SIZE)) / 2)) // = 48

/** @brief Row that contains the horizontal tunnel/portal. */
#define MAP_TUNNEL_ROW 11

/**
 * @brief Tile types that can appear in the map grid.
 */
typedef enum {
    TILE_EMPTY,        /**< Empty walkable tile. */
    TILE_WALL,         /**< Blocking wall tile. */
    TILE_PELLET,       /**< Collectible pellet tile. */
    TILE_POWER_UP,     /**< Collectible power-up tile. */
    TILE_GHOST_SPAWN,  /**< Ghost spawn tile. */
    TILE_PLAYER_SPAWN  /**< Player spawn tile. */
} tile_type_t;

/**
 * @brief One map cell and its collection state.
 */
typedef struct {
    tile_type_t type; /**< Kind of tile stored in this cell. */
    bool collected;   /**< Whether a pellet or power-up has already been collected. */
} tile_t;

/**
 * @brief Complete tile map state.
 */
typedef struct {
    tile_t tiles[MAP_ROWS][MAP_COLS]; /**< Fixed-size map grid. */
    uint16_t pellets_remaining;       /**< Number of remaining pellets/power-ups. */
} map_t;

/**
 * @brief Creates a map using the default layout.
 *
 * @return Created map, or NULL on failure.
 */
map_t *map_create();

/**
 * @brief Frees a map.
 *
 * @param map Map to destroy. NULL is ignored.
 */
void map_destroy(map_t *map);

/**
 * @brief Gets the tile type at a map coordinate.
 *
 * @param map Map to query.
 * @param row Tile row.
 * @param col Tile column.
 * @return Tile type at the coordinate, or TILE_WALL for out-of-bounds.
 */
tile_type_t map_get_tile(const map_t *map, int row, int col);

/**
 * @brief Checks whether a map coordinate can be entered.
 *
 * @param map Map to query.
 * @param row Tile row.
 * @param col Tile column.
 * @return true if the tile is not a wall, false otherwise.
 */
bool map_is_walkable(const map_t *map, int row, int col);

/**
 * @brief Converts a pixel position to map tile coordinates.
 *
 * @param x Pixel x coordinate.
 * @param y Pixel y coordinate.
 * @param row Output tile row.
 * @param col Output tile column.
 */
void map_tile_from_pixel(int x, int y, int *row, int *col);

/**
 * @brief Collects a pellet or power-up at a map coordinate if present.
 *
 * @param map Map to mutate.
 * @param row Tile row.
 * @param col Tile column.
 */
void map_collect_pellet(map_t *map, int row, int col);

/**
 * @brief Checks whether every pellet/power-up has been collected.
 *
 * @param map Map to query.
 * @return true if no collectibles remain, false otherwise.
 */
bool map_all_pellets_collected(const map_t *map);

#endif // _MAP_H_
