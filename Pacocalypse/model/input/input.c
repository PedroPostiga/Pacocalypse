#include "../game/game.h"
#include "input.h"

void input_init(input_state_t *input) {
    if (!input) return;
    input->mouse_x = 0;
    input->mouse_y = 0;
    input->left_button = false;
    input->right_button = false;
    input->middle_button = false;
    input->left_click = false;
}

void input_update(input_state_t *input, const struct packet *mouse_packet) {
    if (!input || !mouse_packet) return;

    int dx = mouse_packet->delta_x;
    int dy = mouse_packet->delta_y;

    if (mouse_packet->bytes[0] & BIT(6)) {
        dx = (dx > 0) ? 127 : -128;
    }
    if (mouse_packet->bytes[0] & BIT(7)) {
        dy = (dy > 0) ? 127 : -128;
    }

    int new_x = input->mouse_x + dx;
    int new_y = input->mouse_y - dy;

    if (new_x < 0) new_x = 0;
    if (new_x > SCREEN_WIDTH - CURSOR_SIZE) new_x = SCREEN_WIDTH - CURSOR_SIZE;
    if (new_y < 0) new_y = 0;
    if (new_y > SCREEN_HEIGHT - CURSOR_SIZE) new_y = SCREEN_HEIGHT - CURSOR_SIZE;

    input->mouse_x = new_x;
    input->mouse_y = new_y;

    bool previous_left = input->left_button;
    input->left_button = (mouse_packet->bytes[0] & BIT(0)) != 0;
    input->right_button = (mouse_packet->bytes[0] & BIT(1)) != 0;
    input->middle_button = (mouse_packet->bytes[0] & BIT(2)) != 0;
    input->left_click = input->left_button && !previous_left;
}

void input_reset_clicks(input_state_t *input) {
    if (!input) return;
    input->left_click = false;
}
