#ifndef _COMMON_H_
#define _COMMON_H_

// #include "rtc.h"





volatile uint32_t _tick;

void TICK_Init(uint32_t sys_tick_value);
void TICK_Inc(void);
uint32_t TICK_Get(void);

void Delay(uint32_t ms);

// Blocking uart logging
// void SEND_LOG(char *text);

int32_t map(int32_t x, int32_t in_min, int32_t in_max, int32_t out_min, int32_t out_max) ;

void F_print00(uint8_t val, char *output);
// void F_print_time(rtc_time_t time, char *output);

// Convert binary format to BCD
uint8_t dec2bcd(uint8_t dec);

// Convert BCD format to BCD binary
uint8_t bcd2dec(uint8_t bcd);

#endif /* _COMMON_H_ */