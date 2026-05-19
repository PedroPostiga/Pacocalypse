// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>
#include "mouse.h"

// Any header files included below this line should have been created by you

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need/ it]
  lcf_trace_calls("/home/lcom/labs/lab4/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab4/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}


int (mouse_test_packet)(uint32_t cnt) {
  int ipc_status, r;
  message msg;

  // enable data reporting (LCF provides this for mouse_test_packet)
  if (mouse_enable_dr() != 0) return 1;
  
  // subscribe mouse interrupts
  uint8_t mouse_bit_no;
  if (mouse_subscribe_int(&mouse_bit_no) != 0) return 1;
  int mouse_irq_set = BIT(mouse_bit_no);
 
  uint32_t packets_read = 0;
  struct packet pp;
 
  while (packets_read < cnt) {
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed: %d\n", r);
      continue;
    }
 
    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if (msg.m_notify.interrupts & mouse_irq_set) {
            mouse_ih();
 
            if (mouse_get_byte_ready()) {
              mouse_set_byte_ready(false);
              uint8_t byte = mouse_get_byte();
 
              if (mouse_parse_packet(byte, &pp)) {
                mouse_print_packet(&pp);
                packets_read++;
              }
            }
          }
        break;
      }
    }
  }

  // unsubscribe before disabling data reporting
  if (mouse_unsubscribe_int() != 0) return 1;

  // disable data reporting - restore mouse to default state
  if (mouse_disable_data_reporting() != 0) return 1;  
 
  return 0;
}

int (mouse_test_async)(uint8_t idle_time) {
    int ipc_status, r;
    message msg;

    // enable data reporting
    if (mouse_enable_dr() != 0) return 1;

    // subscribe mouse interrupts
    uint8_t mouse_bit_no;
    if (mouse_subscribe_int(&mouse_bit_no) != 0) return 1;
    int mouse_irq_set = BIT(mouse_bit_no);
 
    // subscribe timer interrupts
    uint8_t timer_bit_no;
    if (timer_subscribe_int(&timer_bit_no) != 0) return 1;
    int timer_irq_set = BIT(timer_bit_no);
 
    struct packet pp;
    bool done = false;
    int last_packet_count = 0;  // timer counter at last packet received
 
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
                        timer_int_handler();
 
                        // check if idle_time seconds have passed since last packet
                        if ((get_counter() - last_packet_count) >= idle_time * 60)
                            done = true;
                    }
 
                    // mouse interrupt
                    if (msg.m_notify.interrupts & mouse_irq_set) {
                        mouse_ih();
 
                        if (mouse_get_byte_ready()) {
                            mouse_set_byte_ready(false);
                            uint8_t byte = mouse_get_byte();
 
                            if (mouse_parse_packet(byte, &pp)) {
                                mouse_print_packet(&pp);
                                // reset idle timer on every complete packet
                                last_packet_count = get_counter();
                            }
                        }
                    }
                    break;
            }
        }
    }
 
    // unsubscribe both
    if (mouse_unsubscribe_int() != 0) return 1;
    if (timer_unsubscribe_int() != 0) return 1;
 
    // disable data reporting - restore mouse to default state
    if (mouse_disable_data_reporting() != 0) return 1;
 
    return 0;
}
