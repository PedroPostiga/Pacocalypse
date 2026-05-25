#ifndef _VIDEOCARD_H_
#define _VIDEOCARD_H_

#include "lcom/lcf.h"

/* VBE BIOS interrupt */
#define VBE_INTNO       0x10

/* VBE function codes (AX register) */
#define VBE_SET_MODE    0x4F02  /* Set VBE Mode */
#define VBE_GET_MODE_INFO 0x4F01  /* Return VBE Mode Info */
#define VBE_GET_CTRL_INFO 0x4F00  /* Return VBE Controller Info */
#define BIOS_SET_VIDEO_MODE_AH 0x00 /* BIOS Set Video Mode function AH value */
#define VBE_SET_DISPLAY_START 0x4F07 /* Set display start address */
#define SET_DISPLAY_START_BL 0x80

/* VBE BX register flags for Set Mode */
#define VBE_LINEAR_FB   BIT(14) /* Use linear frame buffer */
#define VBE_NO_CLEAR    BIT(15) /* Don't clear display memory (we want clear, so leave unset) */

/* KBC constants for ESC key detection */
#define KBC_STATUS_REG  0x64
#define KBC_OUT_BUF     0x60
#define KBC_OBF         BIT(0)
#define KBC_PARITY_ERR  BIT(7)
#define KBC_TIMEOUT_ERR BIT(6)
#define ESC_BREAK       0x81

int (vge_set_mode)(uint16_t mode);
void* (vg_init)(uint16_t mode);
int (vg_draw_pixel)(uint16_t x, uint16_t y, uint32_t color);
int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color);
int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);
int (vg_draw_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y);
int (vg_flip)();


#endif /* _VIDEOCARD_H_ */
