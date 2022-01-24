/*
*   Here are all interrupts routines except I2C and maybe others that I forget to copy.
*/
#include "stm32g0xx.h"

#include "interrupts.h"
#include "common.h"
#include "gpio.h"
#include "pwr.h"
#include "buttons.h"
#include "recorder.h"





void IT_Init(void)
{
    // Blank function to link file
    return;
}



void SysTick_Handler(void)
{
    TICK_Inc();
}

void EXTI4_15_IRQHandler(void)
{

    if(EXTI->FPR1 & EXTI_FPR1_FPIF4)
    {
        EXTI->FPR1 |= EXTI_FPR1_FPIF4; // Clear flag

        BUTTON_STATUS[BTN_OK].debounce = 1;

        EXTI->IMR1 &= ~EXTI_IMR1_IM4; // Turn off that button interrupt

        TIM14->CNT = 0; // Reset timer counter
    }

    // if(EXTI->FPR1 & EXTI_FPR1_FPIF7)
    // {
    //     EXTI->FPR1 |= EXTI_FPR1_FPIF7; // Clear flag

    //     BUTTON_STATUS[BTN_BACK].debounce = 1;

    //     EXTI->IMR1 &= ~EXTI_IMR1_IM7; // Turn off that button interrupt

    //     TIM14->CNT = 0; // Reset timer counter
    // }

    if(EXTI->FPR1 & EXTI_FPR1_FPIF6)
    {
        EXTI->FPR1 |= EXTI_FPR1_FPIF6; // Clear flag

        BUTTON_STATUS[BTN_UP].debounce = 1;

        EXTI->IMR1 &= ~EXTI_IMR1_IM6; // Turn off that button interrupt

        TIM14->CNT = 0; // Reset timer counter
    }

    if(EXTI->FPR1 & EXTI_FPR1_FPIF5)
    {
        EXTI->FPR1 |= EXTI_FPR1_FPIF5; // Clear flag

        BUTTON_STATUS[BTN_DOWN].debounce = 1;

        EXTI->IMR1 &= ~EXTI_IMR1_IM5; // Turn off that button interrupt

        TIM14->CNT = 0; // Reset timer counter
    }


    TIM14->CR1 |= TIM_CR1_CEN;

}

void TIM14_IRQHandler(void)
{

    TIM14->SR &= ~TIM_SR_UIF; // Clear flag

    TIM14->CR1 &= ~TIM_CR1_CEN; // Stop counter
     
    for (buttons_names_t i = 0; i < BTN_COUNT; i++)
    {
        if (BUTTON_STATUS[i].debounce == 1 && BUTTON_STATUS[i].now_pressed == 0)
        {
            uint8_t state = !(_BTN_CheckState(i));
            if (state == 1)
            {
                BUTTON_STATUS[i].pressed = 0;
                BUTTON_STATUS[i].now_pressed = 1;
                BUTTON_STATUS[i].debounce = 0;
                TIM14->CNT = 0; // Reset timer counter    
                TIM14->CR1 |= TIM_CR1_CEN;
            }
            else
            {
                _BTN_EnableIT(i);
            }
            
        }
        else if(BUTTON_STATUS[i].now_pressed == 1)
        {
            uint8_t state = !(_BTN_CheckState(i));
            
            if (state == 1 || BUTTON_STATUS[i].long_press_timer >= 12)
            {
                if (BUTTON_STATUS[i].long_press_timer >= 12)
                {
                    if (!(_BTN_CheckState(i)))
                    {
                        TIM14->CNT = 0; // Reset timer counter    
                        TIM14->CR1 |= TIM_CR1_CEN;
                    }
                    else
                    {
                        BUTTON_STATUS[i].long_pressed = 1;
                        BUTTON_STATUS[i].pressed = 1;
                        BUTTON_STATUS[i].now_pressed = 0;
                        BUTTON_STATUS[i].long_press_timer = 0;
                        _BTN_EnableIT(i);
                    }   

                }
                else
                {
                    BUTTON_STATUS[i].long_press_timer++;
                    TIM14->CNT = 0; // Reset timer counter    
                    TIM14->CR1 |= TIM_CR1_CEN;
                }

            }
            else
            {
                BUTTON_STATUS[i].now_pressed = 0;
                BUTTON_STATUS[i].long_pressed = 0;
                BUTTON_STATUS[i].pressed = 1;
                BUTTON_STATUS[i].long_press_timer = 0;
                _BTN_EnableIT(i);
            }
        }
    }
}

void DMA1_Channel1_IRQHandler(void)
{

    if(DMA1->ISR & DMA_ISR_TCIF1)
    {
        REC_ProcessDMA();
    }

    DMA1->IFCR = DMA_IFCR_CGIF1 | DMA_IFCR_CTCIF1 | DMA_IFCR_CHTIF1 | DMA_IFCR_CTEIF1;

}

