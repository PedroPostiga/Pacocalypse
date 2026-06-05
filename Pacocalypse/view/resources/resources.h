#ifndef _VIEW_RESOURCES_H_
#define _VIEW_RESOURCES_H_

#include "../sprite.h"
#include "../font.h"

/**
 * @brief All view resources required to draw the game.
 */
typedef struct {
    game_sprites_t *sprites; /**< Loaded sprite collection. */
    font_t *game_font;       /**< Font used by menus and HUD. */
} view_resources_t;

/**
 * @brief Loads sprites and fonts used by the view.
 *
 * @param resources Resource container to initialize.
 * @return 0 on success, non-zero on failure.
 */
int view_resources_init(view_resources_t *resources);

/**
 * @brief Frees all resources owned by a view resource container.
 *
 * @param resources Resource container to clean up.
 */
void view_resources_cleanup(view_resources_t *resources);

#endif // _VIEW_RESOURCES_H_
