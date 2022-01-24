
/*
*   Delay, map, etc.
*/
#include "stm32g0xx.h"

#include "common.h"
#include "gpio.h"

#include "rtc.h"


void TICK_Init(uint32_t sys_tick_value)
{
    SysTick_Config(sys_tick_value);
}

void TICK_Inc(void)
{
    _tick++;
}

uint32_t TICK_Get(void)
{
    return _tick;
}

void Delay(uint32_t ms)
{
    uint32_t start_tick = _tick;

    while ((_tick - start_tick) < ms)
    {
        /* DO NOTHING */
    }
    
}

// void SEND_LOG(char *text)
// {
//     USART2_SendString(text);  
//     USART2_SendString("\r\n");  
// }

int32_t map(int32_t x, int32_t in_min, int32_t in_max, int32_t out_min, int32_t out_max) 
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void F_print00(uint8_t val, char *output)
{
    uint8_t tmp = val / 10;

    if (tmp == 0)
    {
        output[0] = '0';
    }
    else
    {
        output[0] = ('0' + tmp);
    }

    tmp = val % 10;
    
    if (tmp == 0)
    {
        output[1] = '0';
    }
    else
    {
        output[1] = ('0' + tmp);
    }

}

void F_print_time(rtc_time_t time, char *output)
{
    F_print00(time.hour, output);
    output[2] = ':';
    F_print00(time.minute, &output[3]);
    output[5] = ':';
    F_print00(time.second, &output[6]);
}

// Convert binary format to BCD
uint8_t dec2bcd(uint8_t dec)
{
    return ( (dec/10)<<4) | (dec % 10);
}

// Convert BCD format to BCD binary
uint8_t bcd2dec(uint8_t bcd)
{
    return ((((bcd)>>4) & 0x0F) * 10) + ((bcd) & 0x0F);
}