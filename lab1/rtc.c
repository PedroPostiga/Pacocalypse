#include "rtc.h"
#include <stdbool.h>

static int bcd_to_bin(uint8_t bcd) {
    uint8_t lo = bcd & 0x0F;
    uint8_t hi = (bcd >> 4) & 0x0F;
    return (int)(hi * 10 + lo);
}

int rtc_read_date(rtc_date *date) {

    uint8_t is_binary;

    sys_outb(RTC_ADDR_REG, RTC_REG_B);
    uint32_t temp;
    sys_inb(RTC_DATA_REG, &temp);
    is_binary =(temp & RTC_DM_MSK);

    uint8_t uip;

    sys_outb(RTC_ADDR_REG, RTC_REG_A);
    sys_inb(RTC_DATA_REG, &temp);
    uip = (temp & RTC_UIP_MSK);

    while(uip) {
        tickdelay(micros_to_ticks(20000));
        sys_outb(RTC_ADDR_REG, RTC_REG_A);
        sys_inb(RTC_DATA_REG, &temp);
        uip = (temp & RTC_UIP_MSK);}


    sys_outb(RTC_ADDR_REG, RTC_REG_DAY);
    uint32_t day;
    sys_inb(RTC_DATA_REG, &day);

    sys_outb(RTC_ADDR_REG, RTC_REG_MONTH);
    uint32_t month;
    sys_inb(RTC_DATA_REG, &month);

    sys_outb(RTC_ADDR_REG, RTC_REG_YEAR);
    uint32_t year;
    sys_inb(RTC_DATA_REG, &year);

    if(!is_binary){
        int d = bcd_to_bin(day);
        int m = bcd_to_bin(month);
        int y = bcd_to_bin(year);
        if (d < 0 || m < 0 || y < 0) return 1;
        date->day = d;
        date->month = m;
        date->year = y;
    }
    else {
        date->day = (uint8_t)day;
        date->month = (uint8_t)month;
        date->year = (uint8_t)year;
    }

    return 0;
}

static int rtc_wait_valid(uint8_t *is_binary) {
    uint32_t temp;

    if (sys_outb(RTC_ADDR_REG, RTC_REG_B) != 0) return 1;
    if (sys_inb(RTC_DATA_REG, &temp) != 0) return 1;
    *is_binary = (temp & RTC_DM_MSK) != 0;

    do {
        if (sys_outb(RTC_ADDR_REG, RTC_REG_A) != 0) return 1;
        if (sys_inb(RTC_DATA_REG, &temp) != 0) return 1;
        if (!(temp & RTC_UIP_MSK)) return 0;
        tickdelay(micros_to_ticks(20000));
    } while (true);
}

static int rtc_read_register(uint8_t reg, uint8_t *value) {
    uint32_t temp;

    if (sys_outb(RTC_ADDR_REG, reg) != 0) return 1;
    if (sys_inb(RTC_DATA_REG, &temp) != 0) return 1;

    *value = (uint8_t) temp;
    return 0;
}

int rtc_read_time(rtc_time *time) {
    if (!time) return 1;

    uint8_t is_binary;
    if (rtc_wait_valid(&is_binary) != 0) return 1;

    if (rtc_read_register(RTC_REG_SECONDS, &time->seconds) != 0) return 1;
    if (rtc_read_register(RTC_REG_MINUTES, &time->minutes) != 0) return 1;
    if (rtc_read_register(RTC_REG_HOURS, &time->hours) != 0) return 1;

    if (!is_binary) {
        time->seconds = (uint8_t) bcd_to_bin(time->seconds);
        time->minutes = (uint8_t) bcd_to_bin(time->minutes);
        time->hours = (uint8_t) bcd_to_bin(time->hours);
    }

    return 0;
}

uint32_t rtc_time_to_seconds(const rtc_time *time) {
    if (!time) return 0;
    return (uint32_t)time->hours * 3600 + (uint32_t)time->minutes * 60 + time->seconds;
}
