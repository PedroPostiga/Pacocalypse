#ifndef _INPUT_H_
#define _INPUT_H_

#include <stdbool.h>
#include <lcom/lcf.h>
#include "../../../lab4/mouse.h"

typedef struct {
    int mouse_x;
    int mouse_y;
    bool left_button;
    bool right_button;
    bool middle_button;
    bool left_click;
} input_state_t;

void input_init(input_state_t *input);
void input_update(input_state_t *input, const struct packet *mouse_packet);
void input_reset_clicks(input_state_t *input);

#endif // _INPUT_H_
