#ifndef _INPUT_H_
#define _INPUT_H_

#include <stdbool.h>
#include <lcom/lcf.h>
#include "../../../lab4/mouse.h"

/**
 * @brief Controller-owned mouse input state.
 */
typedef struct {
    int mouse_x;         /**< Current mouse x coordinate. */
    int mouse_y;         /**< Current mouse y coordinate. */
    bool left_button;    /**< Whether the left button is currently held. */
    bool right_button;   /**< Whether the right button is currently held. */
    bool middle_button;  /**< Whether the middle button is currently held. */
    bool left_click;     /**< true only when the left button transitions to pressed. */
} input_state_t;

/**
 * @brief Initializes an input state.
 *
 * @param input Input state to initialize.
 */
void input_init(input_state_t *input);

/**
 * @brief Applies a parsed mouse packet to the input state.
 *
 * @param input Input state to update.
 * @param mouse_packet Parsed mouse packet.
 */
void input_update(input_state_t *input, const struct packet *mouse_packet);

/**
 * @brief Clears one-frame click flags after they are handled.
 *
 * @param input Input state to mutate.
 */
void input_reset_clicks(input_state_t *input);

#endif // _INPUT_H_
