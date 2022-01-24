#include "stm32g0xx.h"
#include "stdlib.h"

#include "gpio.h"
#include "adc.h"

#include "ssd1306.h"
#include "BW_GFX.h"

#include "pwr.h"

/*
*   Battery level drawing and sleep enter
*/

void PWR_UpdateBatLevel(void)
{
    uint16_t ADC_SAMPLE = ADC_TakeSample();
    int new_voltage = (((3300*1000)/4096)*ADC_SAMPLE*2)/1000;

    if (abs(new_voltage - batter_voltage) < 20)
    {
        batter_voltage = batter_voltage;
    }
    else
    {
        batter_voltage = new_voltage;
    }
 
}

void _DrawBatteryBar(uint8_t count)
{
    if (count > 4)
    {
        count = 4;
        GFX_DrawFiledRectangle(72, 10, 4, 11, WHITE);
        GFX_DrawFiledRectangle(76, 13, 3, 5, WHITE);
    }
    for (uint8_t i = 0; i < count; i++)
    {
        GFX_DrawFiledRectangle(48 + 6 * i, 10, 5, 11, WHITE);
    }   
}

void PWR_ShowBatStatus(void)
{
    static uint8_t showed_frames;
    static uint8_t blink;

    GFX_DrawFiledRectangle(43, 5, 41, 21, BLACK);
    GFX_DrawRectangle(43, 5, 41, 21, WHITE);

    //Draw battery
    GFX_DrawRectangle(46, 8, 32, 15, WHITE);
    GFX_DrawRectangle(77, 11, 4, 9, WHITE);
    GFX_DrawFiledRectangle(77, 12, 1, 7, BLACK);

    if (batter_voltage > 4100)
    {
        _DrawBatteryBar(5);
    }
    else if (batter_voltage > 4000)
    {
        _DrawBatteryBar(4);
    }
    else if (batter_voltage > 3800)
    {
        _DrawBatteryBar(3);
    }
    else if (batter_voltage > 3600)
    {
        _DrawBatteryBar(2);
    }
    else if (batter_voltage > 3500)
    {
        _DrawBatteryBar(1);
    }
    else
    {
        if (showed_frames % 6 == 0)
        {
            blink++;
        }

        if (blink % 2)
        {
            GFX_DrawFiledRectangle(48, 10, 5, 11, WHITE);
        }
        else
        {
            GFX_DrawFiledRectangle(48, 10, 5, 11, BLACK);
        }
    }

    showed_frames++;

    if (showed_frames > 90)
    {
        showed_frames = 0;
        show_battery_status = 0;
    }
}


void PWR_EnterSleep(int compatibility)
{

    // Turn off other devices power
    GPIO_SetState(GPIO_PORTA, GPIO_PIN13, HIGH);

    // Core sleep mode enable
    SCB->SCR |= (1 << SCB_SCR_SLEEPDEEP_Pos);
    // Sleep mode: standby
    PWR->CR1 |= PWR_CR1_LPMS_0 | PWR_CR1_LPMS_1;
    

    // IDK if this is necessary 
    GPIO_Config(GPIO_PORTB, GPIO_PIN4, GPIO_MODE_ANALOG, GPIO_SPEED_LOW);

    // Procedure from datasheet
    PWR->PUCRB |= PWR_PUCRB_PU5;
    PWR->CR4 |= PWR_CR4_WP6;

    PWR->CR3 |= PWR_CR3_APC | PWR_CR3_EWUP6;

    PWR->SCR |= PWR_SCR_CWUF1 | PWR_SCR_CWUF2 | PWR_SCR_CWUF4  | PWR_SCR_CWUF6;

    __WFI();
}