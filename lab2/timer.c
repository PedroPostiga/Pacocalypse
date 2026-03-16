#include <lcom/lcf.h>
#include <lcom/timer.h>
#include <lcom/lab2.h>

#include <stdint.h>

#include "i8254.h"

int (timer_set_frequency)(uint8_t timer, uint32_t freq) {
  uint8_t conf;
  timer_get_conf(timer, &conf);

  uint8_t timer_sel;
  switch (timer) {
    case 0: timer_sel = TIMER_SEL0; break;
    case 1: timer_sel = TIMER_SEL1; break;
    case 2: timer_sel = TIMER_SEL2; break;
    default: return 1;
  }

  uint8_t ctrl_word = timer_sel | TIMER_LSB_MSB | (conf & 0x0F);
  sys_outb(TIMER_CTRL, ctrl_word);

  uint16_t divisor = (uint16_t)(TIMER_FREQ / freq);

  uint8_t lsb, msb;
  util_get_LSB(divisor, &lsb);
  util_get_MSB(divisor, &msb);

  uint8_t port = TIMER_0 + timer;
  sys_outb(port, lsb);
  sys_outb(port, msb);

  return 0;
}

int (timer_subscribe_int)(uint8_t *bit_no) {
    /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

int (timer_unsubscribe_int)() {
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

void (timer_int_handler)() {
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);
}

int (timer_get_conf)(uint8_t timer, uint8_t *st) {

  uint8_t rb_cmd = TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(timer);
  if (sys_outb(TIMER_CTRL, rb_cmd) != 0)
    return 1;
  return util_sys_inb(TIMER_0 + timer, st);
}

int (timer_display_conf)(uint8_t timer, uint8_t st,
                        enum timer_status_field field) {

  union timer_status_field_val val;

  switch (field) {
    case tsf_all:
      val.byte = st;
      break;
    case tsf_initial: {
      uint8_t raw = (st >> 4) & 0x03;
      switch (raw) {
        case 0: val.in_mode = INVAL_val;     break;
        case 1: val.in_mode = LSB_only;      break;
        case 2: val.in_mode = MSB_only;      break;
        case 3: val.in_mode = MSB_after_LSB; break;
      }
      break;
    }
    case tsf_mode:
      val.count_mode = (st >> 1) & 0x07;
      break;
    case tsf_base:
      val.bcd = (st & 0x01) ? true : false;
      break;
  }

  return timer_print_config(timer, field, val);
}
