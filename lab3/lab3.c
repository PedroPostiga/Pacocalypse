#include <lcom/lcf.h>
#include "../lab2/timer.h"

#include <lcom/lab3.h>

#include "kbc.h"

#include <stdbool.h>
#include <stdint.h>

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab3/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab3/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(kbd_test_scan)() {
  int ipc_status, r;
  message msg;

  uint8_t bit_no;
  if (kbd_subscribe_int(&bit_no)) return 1;

  int irq_set = BIT(bit_no);

  uint8_t bytes[2];
  int size = 0;
  bool two_byte = false;
  bool done = false;

  while (!done) {

    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed: %d\n", r);
      continue;
    }

    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {

        case HARDWARE:
          if (msg.m_notify.interrupts & irq_set) {

            kbc_ih();

            if (get_scancode_status()) {

              set_scancode_status(false);

              uint8_t scancode = get_scancode();

              if (scancode == 0xE0) {
                two_byte = true;
                bytes[0] = scancode;
                size = 1;
              }
              else {
                if (two_byte) {
                  bytes[1] = scancode;
                  size = 2;
                  two_byte = false;
                }
                else {
                  bytes[0] = scancode;
                  size = 1;
                }

                bool make = !(scancode & BIT(7));
                kbd_print_scancode(make, size, bytes);

                if (scancode == ESC_BREAK_CODE) done = true;
              }
            }
          }
        break;
      }
    }
  }

  if (kbd_unsubscribe_int()) return 1;

  return 0;
}

int(kbd_test_poll)() {
  uint8_t bytes[2];
  int size = 0;
  bool two_byte = false;
  bool done = false;


  while (!done) {
    kbc_ih();

    if (get_scancode_status()) {
      set_scancode_status(false);
      uint8_t scancode = get_scancode();

      if (scancode == TWO_BYTE_CODE) {
        two_byte = true;
        bytes[0] = scancode;
        size = 1;
      }
      else {
        if (two_byte) {
          bytes[1] = scancode;
          size = 2;
          two_byte = false;
        }
        else {
          bytes[0] = scancode;
          size = 1;
        }

        bool make = !(scancode & BIT(7));
        kbd_print_scancode(make, size, bytes);

        if (scancode == ESC_BREAK_CODE) done = true;
      }
    }
    else {
      tickdelay(micros_to_ticks(DELAY_US));  // no data yet, wait and retry
    }
  }

  // re-enable KBC interrupts before exiting
  if (kbc_reenable_interrupts() != 0) return 1;

  return 0;
}

int(kbd_test_timed_scan)(uint8_t n) {
  int ipc_status, r;
  message msg;

  // subscribe keyboard interrupts
  uint8_t kbd_bit_no;
  if (kbd_subscribe_int(&kbd_bit_no)) return 1;
  int kbd_irq_set = BIT(kbd_bit_no);

  // subscribe timer interrupts
  uint8_t timer_bit_no;
  if (timer_subscribe_int(&timer_bit_no)) return 1;
  int timer_irq_set = BIT(timer_bit_no);

  uint8_t bytes[2];
  int size = 0;
  bool two_byte = false;
  bool done = false;

  int last_scancode_count = 0;  // timer counter value at last keypress

  while (!done) {
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed: %d\n", r);
      continue;
    }

    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:

          // timer interrupt
          if (msg.m_notify.interrupts & timer_irq_set) {
            timer_ih();

            if ((get_counter() - last_scancode_count) >= n * 60)
              done = true;
          }

          // keyboard interrupt
          if (msg.m_notify.interrupts & kbd_irq_set) {
            kbc_ih();

            if (get_scancode_status()) {
              set_scancode_status(false);
              uint8_t scancode = get_scancode();

              // reset idle timer
              last_scancode_count = get_counter();

              if (scancode == TWO_BYTE_CODE) {
                two_byte = true;
                bytes[0] = scancode;
                size = 1;
              }
              else {
                if (two_byte) {
                  bytes[1] = scancode;
                  size = 2;
                  two_byte = false;
                }
                else {
                  bytes[0] = scancode;
                  size = 1;
                }

                bool make = !(scancode & BIT(7));
                kbd_print_scancode(make, size, bytes);

                if (scancode == ESC_BREAK_CODE) done = true;
              }
            }
          }
        break;
      }
    }
  }

  if (kbd_unsubscribe_int()) return 1;
  if (timer_unsubscribe_int()) return 1;

  return 0;
}
