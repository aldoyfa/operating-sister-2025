#ifndef _CMOS_H 
#define _CMOS_H

#include <stdint.h>

#define CMOS_ADDRESS 0x70
#define CMOS_DATA 0x71

#define CURRENT_YEAR        2025

extern unsigned char second;
extern unsigned char minute;
extern unsigned char hour;
extern unsigned char day;
extern unsigned char month;
extern unsigned int year;

void out_byte(int port, int value);
int in_byte(int port);
 
int get_update_in_progress_flag();
 
unsigned char get_RTC_register(int reg);

uint8_t cmos_read(uint8_t reg);


void read_rtc(uint8_t* hours, uint8_t* minutes, uint8_t* seconds);

#endif 