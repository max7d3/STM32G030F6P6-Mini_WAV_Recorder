#ifndef _RTC_H_
#define _RTC_H_

typedef struct
{
    uint8_t second;
    uint8_t minute;
    uint8_t hour;

    uint8_t day;
    uint8_t month;
    uint8_t year;

}rtc_time_t;

typedef union
{
    rtc_time_t time;
    uint8_t bytes[6];

}rtc_time_u_t;


void RTC_Init(void);

void RTC_SetTime(uint8_t h, uint8_t m, uint8_t s, uint8_t Y, uint8_t M, uint8_t D);

void RTC_GetTime(rtc_time_t *out);

#endif /* _RTC_H_ */