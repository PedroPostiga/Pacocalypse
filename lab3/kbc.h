#include <lcom/lcf.h>

uint8_t get_scancode();

bool get_scancode_status();

void set_scancode_status(bool status);

int (kbd_subscribe_int)(uint8_t *bit_no);
 
int (kbd_unsubscribe_int)();

int (kbc_read_scancode)();

int (kbc_write_cmd)(uint8_t cmd);

int (kbc_read_response)(uint8_t *response);

int (kbc_write_arg)(uint8_t arg);

int (kbc_reenable_interrupts)();


/* ===================== */
/*      IRQ LINE         */
/* ===================== */

#define KBC_IRQ 1   // Keyboard interrupt line (IRQ1)

/* ===================== */
/*      PORTS            */
/* ===================== */

#define KBC_DATA_PORT  0x60  // Output buffer (read) / Input buffer (write)
#define KBC_CMD_REG    0x64  // Command register (write)
#define KBC_STAT_REG   0x64  // Status register (read)

/* ===================== */
/*   STATUS REGISTER     */
/* ===================== */

// Bits of the status register
#define PAR_ERR  BIT(7)  // Parity error
#define TO_ERR   BIT(6)  // Timeout error
#define AUX      BIT(5)  // Mouse data (1 = mouse, 0 = keyboard)
#define INH      BIT(4)  // Inhibit flag
#define A2       BIT(3)  // Not needed
#define SYS      BIT(2)  // Not needed
#define IBF      BIT(1)  // Input buffer full (cannot write)
#define OBF      BIT(0)  // Output buffer full (data available)

/* ===================== */
/*   SCANCODES           */
/* ===================== */

#define ESC_BREAK_CODE 0x81  // ESC key breakcode
#define TWO_BYTE_CODE  0xE0  // Prefix for 2-byte scancodes

/* ===================== */
/*   KBC COMMANDS        */
/* ===================== */

#define READ_CMD_BYTE   0x20
#define WRITE_CMD_BYTE  0x60

/* ===================== */
/*   DELAY               */
/* ===================== */

#define DELAY_US 20000  // 20 ms (used in polling, not interrupts)
