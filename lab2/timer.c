#include <lcom/lcf.h>
#include <lcom/timer.h>
#include <lcom/utils.h>

#include <stdint.h>

#include "i8254.h"

static int hook_id = 0;
int counter = 0;


int (timer_set_frequency)(uint8_t timer, uint32_t freq) {

  if (freq < 19 || freq > TIMER_FREQ){ 
    printf("Invalid frequency\n");
    return 1;
  }

  uint8_t st;
  if (timer_get_conf(timer, &st) != 0) {
    return 1;
  }

  // Preserve bits 0–3 (mode + BCD)
  st = st & 0x0F;

  // Build control word
  uint8_t control = st;

  // Select timer
  switch(timer) {
    case 0: control |= TIMER_SEL0; break;
    case 1: control |= TIMER_SEL1; break;
    case 2: control |= TIMER_SEL2; break;
    default: return 1;
  }

  // Set LSB followed by MSB
  control |= BIT(5) | BIT(4);

  uint16_t divider = TIMER_FREQ / freq;

  uint8_t lsb, msb;
  if(util_get_LSB(divider, &lsb)){
    return 1;
  }

  if(util_get_MSB(divider, &msb)){
    return 1;
  }

  if (sys_outb(TIMER_CTRL, control))
    return 1;

  if (sys_outb(TIMER_0 + timer, lsb)) return 1;
  if (sys_outb(TIMER_0 + timer, msb)) return 1;

  return 0;
}


int (timer_subscribe_int)(uint8_t *bit_no) {
  *bit_no = hook_id;

  if (sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &hook_id)) {
      return 1;
  }

  return 0;
}

int (timer_unsubscribe_int)() {
  if (sys_irqrmpolicy(&hook_id)) {
    return 1;
  }
  return 0;
}

void (timer_int_handler)() {
  counter++;
}

int (timer_get_conf)(uint8_t timer, uint8_t *st) {
  uint32_t rb_cmd = 0;
  rb_cmd |= TIMER_RB_CMD;
  rb_cmd |= TIMER_RB_COUNT_;
  
  switch (timer)
  {
  case 0:
    rb_cmd |= BIT(1);
    break;
  case 1:
    rb_cmd |= BIT(2);
    break;
  case 2:
    rb_cmd |= BIT(3);
    break;
  
  default:
    printf("Invalid Timer");
    return 1;
  }

  if (sys_outb(TIMER_CTRL, rb_cmd)){
    return 1;
  }

  if (util_sys_inb(TIMER_0 + timer, st)) {
    return 1;
  }

  return 0;
}

int (timer_display_conf)(uint8_t timer, uint8_t st, enum timer_status_field field) {

  union timer_status_field_val conf;

  switch(field){
    case tsf_base:
      conf.bcd = st & BIT(0);
      break;

    case tsf_mode:
      conf.count_mode = (st & (BIT(3) | BIT(2) | BIT(1))) >> 1;
      if(conf.count_mode == 6) conf.count_mode = 2;
      if(conf.count_mode == 7) conf.count_mode = 3;
      break;

    case tsf_initial:
      conf.in_mode = (st & (BIT(5) | BIT(4))) >> 4;
      break;  
    
    case tsf_all:
      conf.byte = st;
      break;
    
    default:
      printf("Invalid Field");
      return 1;
  }

  return timer_print_config(timer, field, conf);
}
