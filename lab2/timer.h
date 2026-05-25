#ifndef _TIMER_H_
#define _TIMER_H_

#include "i8254.h"
#include <lcom/lcf.h>

int (get_counter)();
int (timer_set_frequency)(uint8_t timer, uint32_t freq);
int (timer_subscribe_int)(uint8_t *bit_no);
int (timer_unsubscribe_int)();
void (timer_ih)();
int (timer_get_conf)(uint8_t timer, uint8_t *st);

#endif /* _TIMER_H_ */
