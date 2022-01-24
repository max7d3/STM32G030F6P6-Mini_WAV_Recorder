/*
*   Mini WAV recorder on STM32G0 by max7d3
*/

#include "stm32g0xx.h"

#include "stdlib.h"
#include "stdio.h"
#include "string.h"

#include "gpio.h"
#include "adc.h"
#include "rtc.h"
#include "pwr.h"
#include "i2c.h"
#include "spi.h"
#include "i2s.h"

#include "common.h"
#include "interrupts.h"
#include "buttons.h"
#include "fatfs.h"
#include "sdcard.h"
#include "ssd1306.h"
#include "BW_GFX.h"
#include "menu.h"
#include "recorder.h"


// For menu render
uint32_t frame_timer;
uint32_t frame_deley = 34 ;

void CLOCK_Init(void);

void InitAll(void);

int main (void)
{

    InitAll();

    while (1)
    {
        // Draw menu after every 34 ms ~ 30 FPS and update battery level
        if (TICK_Get() - frame_timer > frame_deley)
        {
            
            ssd1306_cls();

            // It could be modified to work without argument but I don't want to change it if it's working.
            MENU_Draw(&menu[menu_status.current_menu]);
            
            ssd1306_display();
        
            frame_timer = TICK_Get();

            PWR_UpdateBatLevel();

        }
        
        // Write data buffer to sd card
        if (recording_in_progress)
        {
            REC_ProcessMain();
        }
    }
    


}

void CLOCK_Init(void)
{
    RCC->PLLCFGR = (0 << RCC_PLLCFGR_PLLM_Pos) // PLL input prescaller = 1
    | (8 << RCC_PLLCFGR_PLLN_Pos) // PLL N value = 8
    | (4 << RCC_PLLCFGR_PLLP_Pos)// PLL P output prescaler = 5  ! FOR I2S !
    | (1 << RCC_PLLCFGR_PLLR_Pos)// PLL R output prescaler = 2
    | RCC_PLLCFGR_PLLREN
    | RCC_PLLCFGR_PLLPEN // ! FOR I2S !
    | RCC_PLLCFGR_PLLSRC_HSI; // PLL clock source = external 

    RCC->CR |= RCC_CR_PLLON; // Turn ON PLL

    FLASH->ACR |= FLASH_ACR_LATENCY_1; // 2WS

    while(!(RCC->CR & RCC_CR_PLLRDY)); // Wait for PLL ready flag

    RCC->CFGR |= RCC_CFGR_SW_1; // Switch system clock to PLL

}

void InitAll(void)
{
    
    // Set system clock to 64Mhz
    CLOCK_Init();

    RTC_Init();

    // It's only to link file with interrupts. Does nothing.
    IT_Init();

    // For Delay etc.
    TICK_Init(64000);

    GPIO_Init();

    // Power up external devices
    GPIO_Config(GPIO_PORTA, GPIO_PIN13, GPIO_MODE_OUTPUT_OPEN_DRAIN, GPIO_SPEED_VERY_HIGH);
    GPIO_SetState(GPIO_PORTA, GPIO_PIN13, LOW);

    // Give them some time to turn on
    Delay(50);

    I2C2_Init(FastPlus);

    // Buttons
    BTN_Init(); 

    ssd1306_init(SSD1306_SWITCHCAPVCC, REFRESH_MAX);
    ssd1306_display();

    SPI2_Init();

    // FatFs from CUBEIDE
    MX_FATFS_Init();

    I2S_Init();

    // Start collecting microphone samples
    REC_Init();

    ADC_Init();


    // Check if sd card is mounted
    if(SDCARD_Mount() == FR_OK)
    {
        sdcard_mounted = 1;
    }
    else
    {
        // try to restart the SD
        sdcard_mounted = 0;
        disk_deinitialize(0);

        if(SDCARD_Mount() == FR_OK)
        {
             sdcard_mounted = 1;
        }
    }

    // If sd card not mounted dispaly error 
    if (sdcard_mounted == 0)
    {
        Delay(33); // Because display buffer transfer may be still in background
        GFX_PrintCentered(12, "SD CARD INIT ERROR", WHITE, BLACK, 1);
        ssd1306_display();
        Delay(2000);
    }
}