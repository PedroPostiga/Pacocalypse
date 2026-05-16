#ifndef _MOUSE_H_
#define _MOUSE_H_

#include "lcom/lcf.h"

/* ===================== */
/*      IRQ LINE         */
/* ===================== */

#define MOUSE_IRQ       12   // Mouse interrupt line (IRQ12)

/* ===================== */
/*      PORTS            */
/* ===================== */

#define MOUSE_DATA_PORT  0x60  // Output buffer (read) / Input buffer (write)
#define MOUSE_CMD_REG    0x64  // Command register (write)
#define MOUSE_STAT_REG   0x64  // Status register (read)

/* ===================== */
/*   STATUS REGISTER     */
/* ===================== */

#define PAR_ERR  BIT(7)  // Parity error
#define TO_ERR   BIT(6)  // Timeout error
#define AUX      BIT(5)  // Mouse data (1 = mouse, 0 = keyboard)
#define IBF      BIT(1)  // Input buffer full (cannot write)
#define OBF      BIT(0)  // Output buffer full (data available)

/* ===================== */
/*   KBC COMMANDS        */
/* ===================== */

#define KBC_WRITE_MOUSE  0xD4  // Forward next byte to mouse

/* ===================== */
/*   MOUSE COMMANDS      */
/* ===================== */

#define MOUSE_RESET         0xFF  // Reset mouse
#define MOUSE_RESEND        0xFE  // Resend last byte
#define MOUSE_SET_DEFAULTS  0xF6  // Set default values
#define MOUSE_DISABLE_DR    0xF5  // Disable data reporting (stream mode)
#define MOUSE_ENABLE_DR     0xF4  // Enable data reporting (stream mode)
#define MOUSE_SET_REMOTE    0xF0  // Set remote mode
#define MOUSE_SET_STREAM    0xEA  // Set stream mode

/* ===================== */
/*   MOUSE RESPONSES     */
/* ===================== */

#define MOUSE_ACK    0xFA  // Command acknowledged
#define MOUSE_NACK   0xFE  // Command not acknowledged - resend
#define MOUSE_ERROR  0xFC  // Error

/* ===================== */
/*   MOUSE PACKET        */
/* ===================== */

#define MOUSE_PACKET_SIZE   3    // Standard PS/2 packet is 3 bytes

// Byte 1 bits
#define MOUSE_LB        BIT(0)  // Left button pressed
#define MOUSE_RB        BIT(1)  // Right button pressed
#define MOUSE_MB        BIT(2)  // Middle button pressed
#define MOUSE_SYNC_BIT  BIT(3)  // Always 1 in byte 1 - used for sync
#define MOUSE_X_SIGN    BIT(4)  // X movement sign bit
#define MOUSE_Y_SIGN    BIT(5)  // Y movement sign bit
#define MOUSE_X_OVF     BIT(6)  // X movement overflow
#define MOUSE_Y_OVF     BIT(7)  // Y movement overflow

/* ===================== */
/*   DELAY               */
/* ===================== */

#define DELAY_US  20000  // 20 ms delay between retries
#define MAX_RETRIES  10  // Maximum number of retries for KBC operations

/* ===================== */
/*   FUNCTION DECLARATIONS */
/* ===================== */

// Interrupt handler
void (mouse_ih)();

// Getters and setters
uint8_t mouse_get_byte();
bool mouse_get_byte_ready();
void mouse_set_byte_ready(bool status);

// Subscribe/unsubscribe
int (mouse_subscribe_int)(uint8_t *bit_no);
int (mouse_unsubscribe_int)();

// KBC communication helpers
int (mouse_write_cmd)(uint8_t cmd);
int (mouse_write_arg)(uint8_t arg);
int (mouse_read_response)(uint8_t *response);

// Mouse command helpers
int (mouse_send_cmd)(uint8_t cmd);
int (mouse_enable_dr)();
int (mouse_disable_data_reporting)();

// Packet parsing
bool mouse_parse_packet(uint8_t byte, struct packet *pp);

#endif
