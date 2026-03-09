#include "bitwise.h"


#define TODO return 255

uint8_t clear(uint8_t msk, int pos) { 
    return msk & (uint8_t)~(1u << pos);
}

uint8_t set(uint8_t msk, int pos) {
    return msk | (uint8_t)(1u << pos);
}

bool is_set(uint8_t msk, int pos) {
    return (msk & (uint8_t)(1u << pos)) != 0;
}

uint8_t lsb(uint16_t wide_msk) {
    return (uint8_t)(wide_msk & 0x00FFu);
}

uint8_t msb(uint16_t wide_msk) {
    return (uint8_t)((wide_msk >> 8) & 0x00FFu);
}

uint8_t mask(int pos, ...) {
    uint8_t m = 0;
    va_list ap;

    va_start(ap, pos);
    while (pos != MSK_END) {                     /* MSK_END is -1 */
        m |= (uint8_t)(1u << pos);               /* set bit */
        pos = va_arg(ap, int);
    }
    va_end(ap);

    return m;
}
