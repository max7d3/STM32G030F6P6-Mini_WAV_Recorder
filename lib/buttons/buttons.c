#include "stm32g0xx.h"

#include "gpio.h"
#include "buttons.h"


/*
*   Buttons on EXTI with debouncing
*/




void BTN_Init(void)
{
    //GPIO_Config(GPIO_PORTA, GPIO_PIN8, GPIO_MODE_INPUT_PULLUP, GPIO_SPEED_LOW);

    GPIO_Config(GPIO_PORTB, GPIO_PIN4, GPIO_MODE_INPUT_PULLUP, GPIO_SPEED_LOW);

    GPIO_Config(GPIO_PORTA, GPIO_PIN6, GPIO_MODE_INPUT_PULLUP, GPIO_SPEED_LOW);
    GPIO_Config(GPIO_PORTA, GPIO_PIN5, GPIO_MODE_INPUT_PULLUP, GPIO_SPEED_LOW);

    RCC->APBENR2 |= RCC_APBENR2_TIM14EN | RCC_APBENR2_SYSCFGEN;

    // EXTI GPIO PORT B
    EXTI->EXTICR[1] = EXTI_EXTICR2_EXTI4_0;

    // Enable interrupt
    EXTI->IMR1 |= EXTI_IMR1_IM4 | /*EXTI_IMR1_IM7 |*/  EXTI_IMR1_IM6 | EXTI_IMR1_IM5;

    // Falling edge trigger
    EXTI->FTSR1 |= EXTI_FTSR1_FT4 | /*EXTI_FTSR1_FT7 |*/  EXTI_FTSR1_FT6 | EXTI_FTSR1_FT5;

    // Timer 14 config
    TIM14->PSC = 6400;
    TIM14->ARR = 250; 
    TIM14->DIER |= TIM_DIER_UIE; // Update interrupt

    TIM14->EGR |= TIM_EGR_UG;
    //TIM14->CR1 |= TIM_CR1_CEN;

    // Enable interrupts ine NVIC
    NVIC_EnableIRQ(EXTI4_15_IRQn);
    // NVIC_EnableIRQ(EXTI4_IRQn);
    NVIC_EnableIRQ(TIM14_IRQn);

}


buttons_sates_t BTN_GetState(buttons_names_t button)
{

    if (BUTTON_STATUS[button].pressed == 0)
    {
        return BTN_STATE_NOT_PRESSED;
    }
    else if (BUTTON_STATUS[button].long_pressed == 1)
    {
        //BUTTON_STATUS[button].long_pressed = 0;
        //BUTTON_STATUS[button].pressed = 0;
        return BTN_STATE_LONG_PRESSED;
    }
    else if (BUTTON_STATUS[button].pressed == 1)
    {
        //BUTTON_STATUS[button].pressed = 0;
        return BTN_STATE_PRESSED;
    }

    return BTN_STATE_NOT_PRESSED;
    
}

void BTN_ClearState(buttons_names_t button)
{
    BUTTON_STATUS[button].long_pressed = 0;
    BUTTON_STATUS[button].pressed = 0;
}

gpio_state_t _BTN_CheckState(buttons_names_t button)
{
    switch (button)
    {

        case BTN_OK:
        return GPIO_GetState(GPIO_PORTB, GPIO_PIN4);

        // case BTN_BACK:
        // return GPIO_GetState(GPIO_PORTA, GPIO_PIN7);

        case BTN_UP:
        return GPIO_GetState(GPIO_PORTA, GPIO_PIN6);
        
        case BTN_DOWN:
        return GPIO_GetState(GPIO_PORTA, GPIO_PIN5);

        default:
        return 0;

    }
}

void _BTN_EnableIT(buttons_names_t button)
{
    switch (button)
    {

        case BTN_OK:
        EXTI->IMR1 |= EXTI_IMR1_IM4;
        break;

        // case BTN_BACK:
        // EXTI->IMR1 |= EXTI_IMR1_IM7;
        // break;

        case BTN_UP:
        EXTI->IMR1 |= EXTI_IMR1_IM6;
        break;
        
        case BTN_DOWN:
        EXTI->IMR1 |= EXTI_IMR1_IM5;
        break;

        default:
        return;

    }
}