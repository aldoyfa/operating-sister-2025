#include "../../header/driver/cmos.h"
#include "../../header/cpu/portio.h"

void out_byte(int port, int value) {
    out((uint16_t)port, (uint8_t)value);
}

int in_byte(int port) {
    return (int)in((uint16_t)port);
}

enum {
      cmos_address = 0x70,
      cmos_data    = 0x71
};
 
int get_update_in_progress_flag() {
    out_byte(cmos_address, 0x0A);
    return (in_byte(cmos_data) & 0x80);
}
 
unsigned char get_RTC_register(int reg) {
    out_byte(cmos_address, reg);
    return in_byte(cmos_data);
}

void read_rtc(unsigned char *hour_ptr, unsigned char *minute_ptr, unsigned char *second_ptr) {
    unsigned char last_second, last_minute, last_hour;
    unsigned char registerB;
    
    // Wait for update to complete
    while (get_update_in_progress_flag());
    
    // Read time registers
    *second_ptr = get_RTC_register(0x00);
    *minute_ptr = get_RTC_register(0x02);
    *hour_ptr = get_RTC_register(0x04);
    
    // Read again to ensure consistency
    do {
        last_second = *second_ptr;
        last_minute = *minute_ptr;
        last_hour = *hour_ptr;
        
        while (get_update_in_progress_flag());
        
        *second_ptr = get_RTC_register(0x00);
        *minute_ptr = get_RTC_register(0x02);
        *hour_ptr = get_RTC_register(0x04);
        
    } while ((last_second != *second_ptr) || (last_minute != *minute_ptr) || (last_hour != *hour_ptr));
    
    // Check register B to see if values are in BCD format
    registerB = get_RTC_register(0x0B);
    
    // Convert BCD to binary values if necessary
    if (!(registerB & 0x04)) {
        *second_ptr = ((*second_ptr / 16) * 10) + (*second_ptr & 0x0F);
        *minute_ptr = ((*minute_ptr / 16) * 10) + (*minute_ptr & 0x0F);
        *hour_ptr = ((*hour_ptr / 16) * 10) + (*hour_ptr & 0x0F);
    }
    
    // Convert 12 hour clock to 24 hour clock if necessary
    if (!(registerB & 0x02) && (*hour_ptr & 0x80)) {
        *hour_ptr = ((*hour_ptr & 0x7F) + 12) % 24;
    }
}

uint8_t cmos_read(uint8_t reg) {
    out(CMOS_ADDRESS, reg);
    return in(CMOS_DATA);
}