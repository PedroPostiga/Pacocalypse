#include <lcom/lcf.h>
#include "videocard.h"
#include <stdint.h>

static char *video_mem; /* Process (virtual) address to which VRAM is mapped */
static vbe_mode_info_t vmi; /* VBE mode info */
static unsigned bytes_per_pixel; /* Number of bytes per pixel */
static bool curr_buffer = 0;
static char *current_buffer; /* Pointer to the currently active buffer (for drawing) */

int (vge_set_mode)(uint16_t mode) {
    reg86_t reg;
    memset(&reg, 0, sizeof(reg));

    reg.intno = VBE_INTNO;
    reg.ax = VBE_SET_MODE;
    reg.bx = mode | VBE_LINEAR_FB;  // set linear framebuffer flag

    if (sys_int86(&reg) != 0) return 1;

    return 0;
}

void* (vg_init)(uint16_t mode) {
    struct minix_mem_range mr;
    unsigned int vram_base;
    unsigned int vram_size;

    // Get VBE mode info
    if (vbe_get_mode_info(mode, &vmi) != 0) return NULL;
    vram_base = vmi.PhysBasePtr;    
    bytes_per_pixel = (vmi.BitsPerPixel + 7) / 8;  // Round up to nearest byte
    vram_size = vmi.XResolution * vmi.YResolution * bytes_per_pixel * 2;

    // Allow memory mapping
    mr.mr_base = (phys_bytes)vram_base;
    mr.mr_limit = mr.mr_base + vram_size;

    if (sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr) != 0) return NULL;

    // Map VRAM to process address space
    video_mem = vm_map_phys(SELF, (void *)mr.mr_base, vram_size);
    if (video_mem == MAP_FAILED) return NULL;

    current_buffer = vram_size / 2 + video_mem; // Start with second half of VRAM for double buffering

    return video_mem;
}

int (vg_draw_pixel)(uint16_t x, uint16_t y, uint32_t color){
    if (x >= vmi.XResolution || y >= vmi.YResolution) return 1; // Out of bounds

    uint32_t pixel_offset = (y * vmi.XResolution + x) * bytes_per_pixel;
    uint8_t *pixel_addr = (uint8_t *)current_buffer + pixel_offset;

    // Write color to VRAM
    for (unsigned i = 0; i < bytes_per_pixel; i++) {
        pixel_addr[i] = (color >> (i * 8)) & 0xFF; // Write each byte of color
    }

    return 0;
}

int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color) {
    for (uint16_t i = 0; i < len; i++) {
        if (vg_draw_pixel(x + i, y, color) != 0) return 1; // Stop if out of bounds
    }
    return 0;
}

int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {
    for (uint16_t j = 0; j < height; j++) {
        if (vg_draw_hline(x, y + j, width, color) != 0) return 1; // Stop if out of bounds
    }
    return 0;
}

int (vg_draw_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y) {
    xpm_image_t img;
    uint8_t *pixmap = xpm_load(xpm, XPM_INDEXED, &img);
    if (!pixmap) return 1;

    for (uint16_t row = 0; row < img.height; row++) {
        for (uint16_t col = 0; col < img.width; col++) {
            uint32_t color = pixmap[row * img.width + col];
            if (vg_draw_pixel(x + col, y + row, color) != 0) return 1; // Stop if out of bounds
        }
    }
    return 0;
}

int (vg_flip)(){
    current_buffer = video_mem; // Switch to front buffer

    reg86_t reg;
    memset(&reg, 0, sizeof(reg));

    reg.intno = VBE_INTNO;
    reg.ax = VBE_SET_DISPLAY_START;
    reg.bh = 0; // Display start in upper half of VRAM
    reg.bl = SET_DISPLAY_START_BL;
    reg.cx = 0; // X offset
    reg.dx = vmi.YResolution; // Y offset

    if (sys_int86(&reg) != 0) return 1;

    return 0;
}
