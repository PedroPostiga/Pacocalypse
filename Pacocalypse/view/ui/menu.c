#include "menu.h"
#include "../renderer/renderer.h"
#include <string.h>

void menu_init(menu_state_t *menu, font_t *font) {
    if (!menu || !font) return;

    menu->play_button.x = (SCREEN_WIDTH - 200) / 2;
    menu->play_button.y = 300;
    menu->play_button.width = 200;
    menu->play_button.height = 50;
    menu->play_button.sp = NULL;
    menu->play_button.hover_sp = NULL;
    menu->play_button.font = font;
    menu->play_button.back_color = CRIMSON_RED;
    menu->play_button.hover_frame_color = DARK_GRAY;
    strcpy(menu->play_button.text, "PLAY");

    menu->quit_button.x = (SCREEN_WIDTH - 200) / 2;
    menu->quit_button.y = 400;
    menu->quit_button.width = 200;
    menu->quit_button.height = 50;
    menu->quit_button.sp = NULL;
    menu->quit_button.hover_sp = NULL;
    menu->quit_button.font = font;
    menu->quit_button.back_color = MILD_GREEN;
    menu->quit_button.hover_frame_color = DARK_GRAY;
    strcpy(menu->quit_button.text, "QUIT");
}

void menu_handle_mouse(menu_state_t *menu, const input_state_t *input, game_state_t *game_state) {
    if (!menu || !input || !game_state) return;

    if (game_state->mode != STATE_MENU || !input->left_click) return;

    if (button_is_hovered(&menu->play_button, input->mouse_x, input->mouse_y)) {
        game_state->mode = STATE_PLAYING;
    } else if (button_is_hovered(&menu->quit_button, input->mouse_x, input->mouse_y)) {
        game_state->mode = STATE_QUIT;
    }
}

void menu_draw(const menu_state_t *menu, const input_state_t *input, const game_sprites_t *sprites) {
    if (!menu || !input || !sprites) return;

    if (sprites->menu_bg) {
        int bg_x = (SCREEN_WIDTH - sprites->menu_bg->width) / 2;
        int bg_y = (SCREEN_HEIGHT - sprites->menu_bg->height) / 2;
        if (bg_x < 0) bg_x = 0;
        if (bg_y < 0) bg_y = 0;
        draw_sprite(sprites->menu_bg, bg_x, bg_y);
    }

    renderer_draw_button(&menu->play_button, input->mouse_x, input->mouse_y);
    renderer_draw_button(&menu->quit_button, input->mouse_x, input->mouse_y);
}
