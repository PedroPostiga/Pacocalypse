#include "kbc.h"
#include "../lab2/timer.h"
#include <lcom/lcf.h>

static uint8_t scancode;
static bool scancode_ready = false;
static int hook_id = 1;

uint8_t get_scancode() {
    return scancode;
}

bool get_scancode_status() {
    return scancode_ready;
}

void set_scancode_status(bool status) {
    scancode_ready = status;
}


int (kbd_subscribe_int)(uint8_t *bit_no) {
    if (sys_irqsetpolicy(KBC_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id) != 0)
        return 1;

    *bit_no = hook_id;

    return 0;
}

void (kbc_ih)() {
    uint8_t status;
    
    if (util_sys_inb(KBC_STAT_REG, &status) != 0)
        return;
    
    if (status & OBF) {
        if (status & (PAR_ERR | TO_ERR))
            return;  // got data but corrupted
        
        util_sys_inb(KBC_DATA_PORT, &scancode);
        scancode_ready = true;
        
        return;  // success
    }
    
    return;  // output buffer empty, try again
}



int (kbd_unsubscribe_int)() {
    if (sys_irqrmpolicy(&hook_id) != 0)
        return 1;

    return 0;
}



int (kbc_write_cmd)(uint8_t cmd) {
    uint8_t status;
    int retries = 10;
    
    while (retries > 0) {
        if (util_sys_inb(KBC_STAT_REG, &status) != 0)
            return 1;
        
        if ((status & IBF) == 0) {  // input buffer is free
            if (sys_outb(KBC_CMD_REG, cmd) != 0)
                return 1;
            return 0;
        }
        
        tickdelay(micros_to_ticks(DELAY_US));
        retries--;
    }
    
    return 1;  // gave up after retries
}

int (kbc_read_response)(uint8_t *response) {
    uint8_t status;
    int retries = 10;
    
    while (retries > 0) {
        if (util_sys_inb(KBC_STAT_REG, &status) != 0)
            return 1;
        
        if (status & OBF) {  // output buffer has data
            if (util_sys_inb(KBC_DATA_PORT, response) != 0)
                return 1;
            
            if (status & (PAR_ERR | TO_ERR))
                return 1;
            
            return 0;
        }
        
        tickdelay(micros_to_ticks(DELAY_US));
        retries--;
    }
    
    return 1;  // gave up
}

int (kbc_write_arg)(uint8_t arg) {
    uint8_t status;
    int retries = 10;
    
    while (retries > 0) {
        if (util_sys_inb(KBC_STAT_REG, &status) != 0)
            return 1;
        
        if ((status & IBF) == 0) {
            if (sys_outb(KBC_DATA_PORT, arg) != 0)  // 0x60 this time
                return 1;
            return 0;
        }
        
        tickdelay(micros_to_ticks(DELAY_US));
        retries--;
    }
    return 1;
}

int (kbc_reenable_interrupts)() {
    uint8_t cmd_byte;
    
    if (kbc_write_cmd(READ_CMD_BYTE) != 0) return 1;
    if (kbc_read_response(&cmd_byte) != 0) return 1;
    
    cmd_byte |= BIT(0);  // set INT bit
    
    if (kbc_write_cmd(WRITE_CMD_BYTE) != 0) return 1;
    if (kbc_write_arg(cmd_byte) != 0) return 1;  // argument to 0x60
    
    return 0;
}
