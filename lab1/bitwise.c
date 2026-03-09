#include "bitwise.h"

#define TODO return 255

uint8_t clear(uint8_t msk, int pos) { return msk & ~(1 << pos); }

uint8_t set(uint8_t msk, int pos) { return msk | (1 << pos); }

bool is_set(uint8_t msk, int pos) { return (msk & (1 << pos)) != 0; }

uint8_t lsb(uint16_t wide_msk) { return (uint8_t)(wide_msk & 0xFF); }

uint8_t msb(uint16_t wide_msk) { return (uint8_t)((wide_msk >> 8) & 0xFF); }

#include <stdarg.h>

uint8_t mask(int pos, ...) {
    /* construct a mask by setting each bit position provided in the
       argument list.  Terminate when MSK_END sentinel is found. */
    uint8_t m = 0;
    va_list ap;
    va_start(ap, pos);

    int p = pos;
    while (p != MSK_END) {
        if (p >= 0 && p < 8) {
            m |= (1 << p);
        }
        p = va_arg(ap, int);
    }
    va_end(ap);
    return m;
}
