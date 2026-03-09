#include "rtc.h"
#include <minix/syslib.h>

#define TODO return -1

#define RTC_ADDR_REG 0x70
#define RTC_DATA_REG 0x71
#define RTC_REG_A 0x0A
#define RTC_REG_B 0x0B
#define RTC_REG_DAY 0x07
#define RTC_REG_MONTH 0x08
#define RTC_REG_YEAR 0x09
#define RTC_UIP_MSK (1 << 7)
#define RTC_DM_MSK (1 << 2)

static int bdc_to_bin(uint8_t bcd) { 
  return ((bcd >> 4) * 10) + (bcd & 0x0F);
}

int rtc_read_date(rtc_date *date) { 

  sys_outb(RTC_ADDR_REG, RTC_REG_A);
  uint32_t temp;
  sys_inb(RTC_DATA_REG, &temp);

  uint32_t uip = temp & RTC_UIP_MSK;

  while (uip) {
    sys_outb(RTC_ADDR_REG, RTC_REG_A);
    sys_inb(RTC_DATA_REG, &temp);
    uip = temp & RTC_UIP_MSK;
  }

  uint32_t day;
  sys_outb(RTC_ADDR_REG, RTC_REG_DAY);
  sys_inb(RTC_DATA_REG, &day);

  uint32_t month;
  sys_outb(RTC_ADDR_REG, RTC_REG_MONTH);
  sys_inb(RTC_DATA_REG, &month);

  uint32_t year;
  sys_outb(RTC_ADDR_REG, RTC_REG_YEAR);
  sys_inb(RTC_DATA_REG, &year);

  uint32_t is_BCD;
  sys_outb(RTC_ADDR_REG, RTC_REG_B);
  sys_inb(RTC_DATA_REG, &is_BCD);

  uint8_t final_day   = (uint8_t) day;
  uint8_t final_month = (uint8_t) month;
  uint8_t final_year  = (uint8_t) year;

  if (!(is_BCD & RTC_DM_MSK)) {
    final_day   = bdc_to_bin(final_day);
    final_month = bdc_to_bin(final_month);
    final_year  = bdc_to_bin(final_year);
  }

  date->day   = final_day;
  date->month = final_month;
  date->year  = final_year;

  return 0;
  
; }