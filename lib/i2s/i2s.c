#include "stm32g0xx.h"

#include "gpio.h"

#include "i2s.h"
/*
* I2S no settings evrything hardcoded
*/


void I2S_Init(void)
{

    GPIO_Config(GPIO_PORTA, GPIO_PIN1, GPIO_MODE_ALTERNATE, GPIO_SPEED_VERY_HIGH);
    GPIO_Config(GPIO_PORTA, GPIO_PIN2, GPIO_MODE_ALTERNATE, GPIO_SPEED_VERY_HIGH);
    GPIO_Config(GPIO_PORTA, GPIO_PIN4, GPIO_MODE_ALTERNATE, GPIO_SPEED_VERY_HIGH);

    RCC->APBENR2 |= RCC_APBENR2_SPI1EN; // Enable SPI1 clock

    RCC->AHBENR |= RCC_AHBENR_DMA1EN; // Enable DMA1 clock

    RCC->CCIPR |= RCC_CCIPR_I2S1SEL_0; //I2S colock source is PLL output P

    SPI1->I2SPR = 0x10C; // Configuration value got from cube ide

    SPI1->I2SCFGR = SPI_I2SCFGR_CHLEN | SPI_I2SCFGR_DATLEN_0 | SPI_I2SCFGR_I2SCFG_0 | SPI_I2SCFGR_I2SCFG_1 | SPI_I2SCFGR_I2SMOD;

    SPI1->CR2 |= SPI_CR2_RXDMAEN;

    SPI1->I2SCFGR |= SPI_I2SCFGR_I2SE;

    // DMA config
    DMA1_Channel1->CCR = DMA_CCR_PL_0 | DMA_CCR_PL_1 | DMA_CCR_MSIZE_0 | DMA_CCR_PSIZE_0 | DMA_CCR_MINC | DMA_CCR_CIRC | DMA_CCR_TCIE;

    DMAMUX1_Channel0->CCR = 0x10; // Request ID
    
    NVIC_EnableIRQ(DMA1_Channel1_IRQn);

    DMA1->IFCR = DMA1->ISR; // Clear all DMA flags

}

void I2S_Read(uint16_t *output, uint16_t data_size)
{
    uint8_t sync = 0;
    uint8_t sync_not_complete = 1;
    data_size = data_size * 2;
    // Clear flag
    uint32_t temp = SPI1->DR;
    temp = SPI1->SR;
    temp = temp;

    while (sync_not_complete)
    {
        while (!(SPI1->SR  & SPI_SR_RXNE));

        if (SPI1->SR & SPI_SR_CHSIDE)
        {
            uint16_t temp_rx = (uint16_t)SPI1->DR;
            temp_rx = temp_rx;
            sync = 0;
        }
        else
        {
            uint16_t temp_rx = (uint16_t)SPI1->DR;
            temp_rx = temp_rx;
            sync++;
        }

        if (sync == 2)
        {
            sync_not_complete = 0;
        }
        
    }

    while (data_size > 0)
    {  
        while (!(SPI1->SR  & SPI_SR_RXNE));

        *output++ = (uint16_t)SPI1->DR;
        data_size--;    
    }
    
}


void I2S_StartDMA(uint16_t *output, uint16_t data_size)
{
    uint8_t sync = 0;
    uint8_t sync_not_complete = 1;
    data_size = data_size * 2;
    // Clear flag
    uint32_t temp = SPI1->DR;
    temp = SPI1->SR;
    temp = temp;

    while (sync_not_complete)
    {
        while (!(SPI1->SR  & SPI_SR_RXNE));

        if (SPI1->SR & SPI_SR_CHSIDE)
        {
            uint16_t temp_rx = (uint16_t)SPI1->DR;
            temp_rx = temp_rx;
            sync++;
        }
        else
        {
            uint16_t temp_rx = (uint16_t)SPI1->DR;
            temp_rx = temp_rx;
            sync = 0;
        }

        if (sync == 2)
        {
            sync_not_complete = 0;
        }
        
    }
    DMA1_Channel1->CPAR = (uint32_t)&SPI1->DR;
    DMA1_Channel1->CMAR = (uint32_t)output;
    DMA1_Channel1->CNDTR = data_size;

    DMA1_Channel1->CCR |= DMA_CCR_EN;
    
}

void I2S_StopDMA(void)
{
    // Force DMA shutdown
    // DMA1_Stream3->CR &= ~DMA_SxCR_EN;
    //SPI1->I2SCFGR &= ~SPI_I2SCFGR_I2SE;
}