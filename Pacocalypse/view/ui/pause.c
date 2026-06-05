#include "pause.h"
#include "../renderer/renderer.h"
#include "../../config.h"
#include <string.h>

void pause_init(pause_state_t *pause, font_t *font) {
    if (!pause || !font) return;

    pause->paused_button.x = (SCREEN_WIDTH - 200) / 2;
    pause->paused_button.y = 300;
    pause->paused_button.width = 200;
    pause->paused_button.height = 50;
    pause->paused_button.sp = NULL;
    pause->paused_button.hover_sp = NULL;
    pause->paused_button.font = font;
    pause->paused_button.back_color = DARK_GRAY;
    pause->paused_button.hover_frame_color = CRIMSON_RED;
    strcpy(pause->paused_button.text, "PAUSED");

    pause->quit_button.x = (SCREEN_WIDTH - 200) / 2;
    pause->quit_button.y = 400;
    pause->quit_button.width = 200;
    pause->quit_button.height = 50;
    pause->quit_button.sp = NULL;
    pause->quit_button.hover_sp = NULL;
    pause->quit_button.font = font;
    pause->quit_button.back_color = MILD_GREEN;
    pause->quit_button.hover_frame_color = DARK_GRAY;
    strcpy(pause->quit_button.text, "QUIT");
}

void pause_draw(const pause_state_t *pause, int mouse_x, int mouse_y, const game_sprites_t *sprites) {
    (void) sprites;

    if (!pause) return;

    renderer_draw_button(&pause->paused_button, mouse_x, mouse_y);
    renderer_draw_button(&pause->quit_button, mouse_x, mouse_y);
}
