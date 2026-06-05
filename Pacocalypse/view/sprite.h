#ifndef _SPRITE_H_
#define _SPRITE_H_

#include <stdint.h>
#include <stdlib.h>
#include <lcom/lcf.h>


/**
 * @brief Static bitmap sprite loaded from an XPM.
 */
typedef struct {
    uint8_t *pixmap;          /**< Pixel buffer owned by the sprite. */
    uint16_t width, height;   /**< Sprite dimensions in pixels. */
} sprite_t;

/**
 * @brief Sprite animation made of multiple bitmap frames.
 */
typedef struct {
    uint8_t no_pixmaps;       /**< Number of animation frames. */
    uint8_t current_pixmap;   /**< Index of the currently displayed frame. */
    sprite_t **frames;        /**< Array of frame sprites. */
    uint16_t x, y;            /**< Optional screen position used by animated drawing helpers. */
    uint8_t ticks_per_frame;  /**< Game ticks to show each frame. */
    uint8_t tick_count;       /**< Tick counter used for frame switching. */
} animated_sprite_t;

/**
 * @brief Collection of all game sprites and sprite animations.
 */
typedef struct {
    sprite_t* menu_bg;                         /**< Menu background image. */
    sprite_t* map;                             /**< Map/background sprite. */
    sprite_t* pebble;                          /**< Pellet sprite. */
    sprite_t* power_up;                        /**< Power-up collectible sprite. */
    sprite_t* cursor;                          /**< Mouse cursor sprite. */
    animated_sprite_t* ghost_eyes;             /**< Eyes animation used by dead/respawning ghosts. */
    animated_sprite_t* player_anim_up;         /**< Player animation facing up. */
    animated_sprite_t* player_anim_down;       /**< Player animation facing down. */
    animated_sprite_t* player_anim_left;       /**< Player animation facing left. */
    animated_sprite_t* player_anim_right;      /**< Player animation facing right. */
    animated_sprite_t* ghost_cyan;             /**< Cyan ghost animation. */
    animated_sprite_t* ghost_red;              /**< Red ghost animation. */
    animated_sprite_t* ghost_pink;             /**< Pink ghost animation. */
    animated_sprite_t* ghost_orange;           /**< Orange ghost animation. */
    animated_sprite_t* frightened_ghost_anim;  /**< Frightened ghost animation. */
} game_sprites_t;

/**
 * @brief Creates a sprite from an XPM map.
 *
 * @param sprite XPM image data.
 * @return Created sprite, or NULL on failure.
 */
sprite_t *sprite_create(xpm_map_t sprite);

/**
 * @brief Frees a sprite and its pixel data.
 *
 * @param sprite Sprite to destroy. NULL is ignored.
 */
void sprite_destroy(sprite_t* sprite);

/**
 * @brief Creates an animated sprite from a set of XPM frames.
 *
 * @param xpms Array of XPM frame maps.
 * @param num_frames Number of frames in xpms.
 * @param ticks_per_frame Game ticks each frame should remain visible.
 * @param x Initial screen x position.
 * @param y Initial screen y position.
 * @return Created animated sprite, or NULL on failure.
 */
animated_sprite_t *animated_sprite_create(xpm_map_t xpms[], uint8_t num_frames,uint8_t ticks_per_frame, uint16_t x, uint16_t y);

/**
 * @brief Frees an animated sprite and all owned frame sprites.
 *
 * @param sprite Animated sprite to destroy. NULL is ignored.
 */
void animated_sprite_destroy(animated_sprite_t* sprite);

/**
 * @brief Advances an animated sprite according to its tick rate.
 *
 * @param sprite Animated sprite to update.
 */
void animated_sprite_update(animated_sprite_t* sprite);

/**
 * @brief Gets the currently visible frame of an animated sprite.
 *
 * @param sprite Animated sprite to query.
 * @return Current frame sprite, or NULL if unavailable.
 */
sprite_t* animated_sprite_get_current_frame(animated_sprite_t* sprite);

/**
 * @brief Loads every sprite resource used by the game.
 *
 * @param sprites Output pointer that receives the allocated sprite collection.
 * @return 0 on success, non-zero on failure.
 */
int load_sprites(game_sprites_t **sprites);

/**
 * @brief Frees a sprite collection created by load_sprites().
 *
 * @param sprites Address of the sprite collection pointer to destroy and clear.
 */
void destroy_sprites(game_sprites_t **sprites);

#endif // _SPRITE_H_
