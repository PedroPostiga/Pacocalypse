#include "mouse.h"

static uint8_t mouse_byte;
static bool mouse_byte_ready = false;
static int hook_id = 12;


uint8_t mouse_get_byte() {
    return mouse_byte;
}

bool mouse_get_byte_ready() {
    return mouse_byte_ready;
}

void mouse_set_byte_ready(bool status) {
    mouse_byte_ready = status;
}


int (mouse_ih)() {
    uint8_t status;

    if (util_sys_inb(MOUSE_STAT_REG, &status) != 0)
        return 1;

    if (!(status & OBF) || !(status & AUX))
        return 1;

    // always read to clear buffer
    uint8_t byte;
    if (util_sys_inb(MOUSE_DATA_PORT, &byte) != 0)
        return 1;

    // only store if no errors
    if (status & (PAR_ERR | TO_ERR))
        return 1;

    mouse_byte = byte;
    mouse_byte_ready = true;

    return 0;
}


int (mouse_subscribe_int)(uint8_t *bit_no) {
    *bit_no = hook_id;

    if (sys_irqsetpolicy(MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id) != 0)
        return 1;

    return 0;
}

int (mouse_unsubscribe_int)() {
    if (sys_irqrmpolicy(&hook_id) != 0)
        return 1;

    return 0;
}


// Write a command to the KBC command register (0x64)
// Waits until input buffer is free before writing
int (mouse_write_cmd)(uint8_t cmd) {
    uint8_t status;
    int retries = MAX_RETRIES;

    while (retries > 0) {
        if (util_sys_inb(MOUSE_STAT_REG, &status) != 0)
            return 1;

        if ((status & IBF) == 0) {
            if (sys_outb(MOUSE_CMD_REG, cmd) != 0)
                return 1;
            return 0;
        }

        tickdelay(micros_to_ticks(DELAY_US));
        retries--;
    }

    return 1;  // gave up after retries
}

// Write an argument to the data port (0x60)
// Waits until input buffer is free before writing
int (mouse_write_arg)(uint8_t arg) {
    uint8_t status;
    int retries = MAX_RETRIES;

    while (retries > 0) {
        if (util_sys_inb(MOUSE_STAT_REG, &status) != 0)
            return 1;

        if ((status & IBF) == 0) {
            if (sys_outb(MOUSE_DATA_PORT, arg) != 0)
                return 1;
            return 0;
        }

        tickdelay(micros_to_ticks(DELAY_US));
        retries--;
    }

    return 1;
}

// Read a response from the output buffer (0x60)
// Waits until output buffer is full before reading
int (mouse_read_response)(uint8_t *response) {
    uint8_t status;
    int retries = MAX_RETRIES;

    while (retries > 0) {
        if (util_sys_inb(MOUSE_STAT_REG, &status) != 0)
            continue;

        if (status & OBF) {
            if (util_sys_inb(MOUSE_DATA_PORT, response) != 0)
                continue;

            if (status & (PAR_ERR | TO_ERR))
                continue;

            if (!(status & AUX))
                continue;  // not from mouse, ignore
        }

        tickdelay(micros_to_ticks(DELAY_US));
        retries--;
    }

    return 1;
}


// Send a command byte directly to the mouse via KBC
// Handles ACK/NACK and retries on error
int (mouse_send_cmd)(uint8_t cmd) {
    int retries = MAX_RETRIES;

    while (retries > 0) {
        // step 1: tell KBC to forward next byte to mouse
        if (mouse_write_cmd(KBC_WRITE_MOUSE) != 0){
            retries--;
            tickdelay(micros_to_ticks(DELAY_US));
            continue;
        }

        // step 2: write the actual mouse command
        if (mouse_write_arg(cmd) != 0){
            retries--;
            tickdelay(micros_to_ticks(DELAY_US));
            continue;
        }

        // step 3: read acknowledgement from mouse
        uint8_t ack;
        if (mouse_read_response(&ack) != 0){
            retries--;
            tickdelay(micros_to_ticks(DELAY_US));
            continue;
        }

        if (ack == MOUSE_ACK)
            return 0;  // success

        if (ack == MOUSE_ERROR)
            return 1;  // unrecoverable error

        // ack == MOUSE_NACK (0xFE) - retry entire command
        retries--;
        tickdelay(micros_to_ticks(DELAY_US));
    }

    return 1;
}

int (mouse_enable_dr)() {
    return mouse_send_cmd(MOUSE_ENABLE_DR);
}

int (mouse_disable_data_reporting)() {
    return mouse_send_cmd(MOUSE_DISABLE_DR);
}


// Call with each byte received in order (byte 0, 1, 2)
// Returns true when a full packet has been parsed and stored in pp
bool mouse_parse_packet(uint8_t byte, struct packet *pp) {
    static uint8_t packet_buf[MOUSE_PACKET_SIZE];
    static int packet_idx = 0;

    // synchronization: byte 0 must have bit 3 set
    if (packet_idx == 0 && !(byte & MOUSE_SYNC_BIT))
        return false;  // not in sync, discard

    packet_buf[packet_idx] = byte;
    packet_idx++;

    if (packet_idx < MOUSE_PACKET_SIZE)
        return false;  // packet not complete yet

    // full packet received - parse it
    packet_idx = 0;

    pp->bytes[0] = packet_buf[0];
    pp->bytes[1] = packet_buf[1];
    pp->bytes[2] = packet_buf[2];

    pp->lb = packet_buf[0] & MOUSE_LB;
    pp->rb = packet_buf[0] & MOUSE_RB;
    pp->mb = packet_buf[0] & MOUSE_MB;

    pp->x_ov = packet_buf[0] & MOUSE_X_OVF;
    pp->y_ov = packet_buf[0] & MOUSE_Y_OVF;

    // X movement: if sign bit set, extend to negative using two's complement
    pp->delta_x = packet_buf[1];
    if (packet_buf[0] & MOUSE_X_SIGN)
        pp->delta_x |= 0xFF00;  // sign extend to 16-bit negative

    // Y movement: same as X
    pp->delta_y = packet_buf[2];
    if (packet_buf[0] & MOUSE_Y_SIGN)
        pp->delta_y |= 0xFF00;

    return true;  // packet complete and parsed
}
