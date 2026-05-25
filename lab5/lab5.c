// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <lcom/lab5.h>

#include <stdint.h>
#include <stdio.h>

// Any header files included below this line should have been created by you

#include "videocard.h"
#include "../lab3/kbc.h"
#include "../lab2/timer.h"

int main(int argc, char *argv[]) {
    // sets the language of LCF messages (can be either EN-US or PT-PT)
    lcf_set_language("EN-US");

    // enables to log function invocations that are being "wrapped" by LCF
    // [comment this out if you don't want/need it]
    lcf_trace_calls("/home/lcom/labs/lab5/trace.txt");

    // enables to save the output of printf function calls on a file
    // [comment this out if you don't want/need it]
    lcf_log_output("/home/lcom/labs/lab5/output.txt");

    // handles control over to LCF
    // [LCF handles command line arguments and invokes the right function]
    if (lcf_start(argc, argv))
        return 1;

    // LCF clean up tasks
    // [must be the last statement before return]
    lcf_cleanup();

    return 0;
}

int(video_test_init)(uint16_t mode, uint8_t delay) {
    if (vge_set_mode(mode) != 0)
        return 1;
    
    sleep(delay);

     // Restore text mode (0x03) before exiting
    
    if (vg_exit() != 0)
        return 1;

    return 0;
}

int(video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y,
                          uint16_t width, uint16_t height, uint32_t color) {
    if (vg_init(mode) == NULL)
        return 1;
    
    if (vge_set_mode(mode) != 0)
        return 1;
        
    if (vg_draw_rectangle(x, y, width, height, color) != 0)
        return 1;

    if (vg_flip() != 0)
        return 1;
    
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

                if (scancode == ESC_BREAK_CODE) done = true;
              }
            }
          }
        break;
      }
    }
  }

  if (kbd_unsubscribe_int()) return 1;

     // Restore text mode (0x03) before exiting
    if (vg_exit() != 0)
        return 1;

    return 0;
}

int(video_test_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y) {
    if (vg_init(0x105) == NULL)
        return 1;
    
    if (vge_set_mode(0x105) != 0)
        return 1;
        
    if (vg_draw_xpm(xpm, x, y) != 0)
        return 1;
    
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

                            if (scancode == ESC_BREAK_CODE) done = true;
                    }
                    }
                }
                break;
            }
        }
    }

    if (kbd_unsubscribe_int()) return 1;

    // Restore text mode (0x03) before exiting
    if (vg_exit() != 0)
        return 1;

    return 0;
}
