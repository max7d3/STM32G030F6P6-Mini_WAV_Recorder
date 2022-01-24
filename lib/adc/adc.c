#include "stm32g0xx.h"

#include "gpio.h"
#include "common.h"

#include "adc.h"


void ADC_Init(void)
{
    // Enable ADC clcok
    RCC->APBENR2 |= RCC_APBENR2_ADCEN;

    // Configure GPIOA - 7 as analog input
    GPIO_Config(GPIO_PORTA, GPIO_PIN7, GPIO_MODE_ANALOG, GPIO_SPEED_LOW);

    // Continous conversion and overwrite data
    ADC1->CFGR1 |= ADC_CFGR1_CONT | ADC_CFGR1_OVRMOD;

    // ADC clock = PCLK/2 and max oversampling
    ADC1->CFGR2 |= ADC_CFGR2_CKMODE_0 | ADC_CFGR2_OVSR_0 | ADC_CFGR2_OVSR_1 | ADC_CFGR2_OVSR_2 | ADC_CFGR2_OVSS_3 | ADC_CFGR2_OVSE;

    // ADC sampling time = maximum
    ADC1->SMPR |= ADC_SMPR_SMP1_0 | ADC_SMPR_SMP1_1 | ADC_SMPR_SMP1_2;

    // ADC select channel 7
    ADC1->CHSELR = ADC_CHSELR_CHSEL7;

    // Calibrate ADC
    ADC1->CR |= ADC_CR_ADVREGEN; 

    // Wait for regulator startup
    Delay(1);

    // Begin calibration
    ADC1->CR |= ADC_CR_ADCAL;

    // Wait for complete
    while (!(ADC1->ISR & ADC_ISR_EOCAL));

    // Clear flag
    ADC1->ISR |= ADC_ISR_EOCAL;

    // Enable ADC
    ADC1->CR |= ADC_CR_ADEN;

    // Wait for ready flag
    while (!(ADC1->ISR & ADC_ISR_ADRDY));

    // Clear flag
    ADC1->ISR |= ADC_ISR_ADRDY;

    //Start ADC
    ADC1->CR |= ADC_CR_ADSTART;

}

uint16_t ADC_TakeSample(void)
{
    return ADC1->DR;
}