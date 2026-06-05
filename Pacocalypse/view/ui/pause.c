#include "pause.h"
#include "../renderer/renderer.h"
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
}

void pause_handle_mouse(pause_state_t *pause, const input_state_t *input, game_state_t *game_state) {
    if (!pause || !input || !game_state) return;

    if (game_state->mode != STATE_PAUSED || !input->left_click) return;

    if (button_is_hovered(&pause->paused_button, input->mouse_x, input->mouse_y)) {
        game_state->mode = STATE_PLAYING;
    }
}

void pause_draw(const pause_state_t *pause, const input_state_t *input, const game_sprites_t *sprites) {
    (void) sprites;

    if (!pause || !input) return;

    renderer_draw_button(&pause->paused_button, input->mouse_x, input->mouse_y);
}
