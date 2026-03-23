#include <lcom/lcf.h>

#include <stdint.h>

int(util_get_LSB)(uint16_t val, uint8_t *lsb) {
  *lsb = val & 0xFF;
  return 0;
}

int(util_get_MSB)(uint16_t val, uint8_t *msb) {
  *msb = (val >> 8) & 0xFF;
  return 0;
}

int (util_sys_inb)(int port, uint8_t *value) {
  uint32_t stub;

  if (sys_inb(port, &stub)){
    return 1;
  }

  *value = (uint8_t) stub;
  return 0;
}
