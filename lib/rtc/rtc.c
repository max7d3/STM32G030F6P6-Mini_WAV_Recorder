#include "stm32g0xx.h"

#include "rtc.h"

/*
*   It could be done better but I made this some time ago and it works 
*/

void RTC_Init(void)
{
    RCC->APBENR1 |= RCC_APBENR1_PWREN;

    PWR->CR1 |= PWR_CR1_DBP;

    RCC->BDCR |= RCC_BDCR_LSEON;
    while(!(RCC->BDCR & RCC_BDCR_LSERDY)); // Wait for ready flag


    RCC->BDCR |= RCC_BDCR_RTCEN | RCC_BDCR_RTCSEL_0;

    RCC->APBENR1 |= RCC_APBENR1_RTCAPBEN;

    // Enter RTC key to unlock registers
    RTC->WPR = 0xCA;
    RTC->WPR = 0x53;

}

void RTC_SetTime(uint8_t h, uint8_t m, uint8_t s, uint8_t Y, uint8_t M, uint8_t D)
{
    RTC->ICSR |= RTC_ICSR_INIT; // Init mode
    while(!(RTC->ICSR & RTC_ICSR_INITF));

    RTC->TR = ((s-((s/10)*10)) & RTC_TR_SU_Msk) | (((s/10)<<RTC_TR_ST_Pos) & RTC_TR_ST_Msk)
    | (((m-((m/10)*10))<<RTC_TR_MNU_Pos) & RTC_TR_MNU_Msk) | (((m/10)<<RTC_TR_MNT_Pos) & RTC_TR_MNT_Msk)
    | (((h-((h/10)*10))<<RTC_TR_HU_Pos) & RTC_TR_HU_Msk) | (((h/10)<<RTC_TR_HT_Pos) & RTC_TR_HT_Msk);

    RTC->DR = ((D-((D/10)*10)) & RTC_DR_DU_Msk) | (((D/10)<<RTC_DR_DT_Pos) & RTC_DR_DT_Msk)
    | (((M-((M/10)*10))<<RTC_DR_MU_Pos) & RTC_DR_MU_Msk) | (((M/10)<<RTC_DR_MT_Pos) & RTC_DR_MT_Msk)
    | (((Y-((Y/10)*10))<<RTC_DR_YU_Pos) & RTC_DR_YU_Msk) | (((Y/10)<<RTC_DR_YT_Pos) & RTC_DR_YT_Msk);


    RTC->ICSR &= ~RTC_ICSR_INIT; // Init mode exit
    while(RTC->ICSR & RTC_ICSR_INITF);
}

void RTC_GetTime(rtc_time_t *out)
{
    out->second = (RTC->TR & RTC_TR_SU_Msk) + (((RTC->TR & RTC_TR_ST_Msk)>>RTC_TR_ST_Pos)*10);
    out->minute = ((RTC->TR & RTC_TR_MNU_Msk)>>RTC_TR_MNU_Pos) + (((RTC->TR & RTC_TR_MNT_Msk)>>RTC_TR_MNT_Pos)*10);
    out->hour = ((RTC->TR & RTC_TR_HU_Msk)>>RTC_TR_HU_Pos) + (((RTC->TR & RTC_TR_HT_Msk)>>RTC_TR_HT_Pos)*10);
    out->day = (RTC->DR & RTC_DR_DU_Msk) + (((RTC->DR & RTC_DR_DT_Msk)>>RTC_DR_DT_Pos)*10);
    out->month = ((RTC->DR & RTC_DR_MU_Msk)>>RTC_DR_MU_Pos) + (((RTC->DR & RTC_DR_MT_Msk)>>RTC_DR_MT_Pos)*10);
    out->year = ((RTC->DR & RTC_DR_YU_Msk)>>RTC_DR_YU_Pos) + (((RTC->DR & RTC_DR_YT_Msk)>>RTC_DR_YT_Pos)*10);  
}

