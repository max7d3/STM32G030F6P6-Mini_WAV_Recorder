#include "stm32g0xx.h"

#include "gpio.h"

#include "spi.h"

/*
*  I don't think this need explanation
*/


void SPI2_Init(void)
{
    RCC->APBENR1 |= RCC_APBENR1_SPI2EN; // Enable SPI2 clock

    GPIO_Config(GPIO_PORTA, GPIO_PIN3, GPIO_MODE_ALTERNATE, GPIO_SPEED_VERY_HIGH);
    GPIO_Config(GPIO_PORTA, GPIO_PIN0, GPIO_MODE_ALTERNATE, GPIO_SPEED_VERY_HIGH);
    GPIO_Config(GPIO_PORTB, GPIO_PIN7, GPIO_MODE_ALTERNATE, GPIO_SPEED_VERY_HIGH);

    // For CS pin
    GPIO_Config(GPIO_PORTA, GPIO_PIN8, GPIO_MODE_OUTPUT, GPIO_SPEED_VERY_HIGH);



    GPIOB->AFR[0] |=  (1 << GPIO_AFRL_AFSEL7_Pos);

    SPI2->CR1 = SPI_CR1_BR_0 | SPI_CR1_BR_1 | SPI_CR1_BR_2 // Minimal speed for init SPI_CR1_BR_0 | SPI_CR1_BR_1 | SPI_CR1_BR_2
    | SPI_CR1_SSM | SPI_CR1_SSI // Softwere CS
    | SPI_CR1_MSTR; // Master mode

    SPI2->CR2 |= SPI_CR2_DS_0 | SPI_CR2_DS_1 | SPI_CR2_FRXTH; // Data to transmit size 8 bits

    SPI2->CR1 |= SPI_CR1_SPE; // Enable SPI

}

uint8_t SPI2_ExchangeByte(uint8_t tx_byte)
{
    uint8_t rx_byte;

    while (!(SPI2->SR & SPI_SR_TXE));
    *(__IO uint8_t *)&SPI2->DR = tx_byte;

    while (!(SPI2->SR  & SPI_SR_RXNE));
    rx_byte = *(__IO uint8_t *)&SPI2->DR;

    // Wait until not SPI is not busy
    while ((SPI2->SR  & SPI_SR_BSY));

    // Clear falgs
    uint8_t temp = SPI2->DR;
    temp = SPI2->SR;
    temp = temp;

    return rx_byte;
}

void SPI2_Tx(uint8_t *data, uint16_t data_size)
{
    while (data_size > 0)
    {
        while (!(SPI2->SR & SPI_SR_TXE));
        *(__IO uint8_t *)&SPI2->DR = *data++;
        data_size--;
        while ((SPI2->SR  & SPI_SR_BSY));
    }

    while ((SPI2->SR  & SPI_SR_BSY));

    //Clear FIFO
    while ((SPI2->SR  & SPI_SR_FRLVL))
    {
        uint8_t temp = *(__IO uint8_t *)&SPI2->DR;
        temp = temp;
    }

    //Clear falgs
    volatile uint32_t temp = SPI2->DR;
    temp = SPI2->SR;
    temp = temp; 

}

void SPI2_Rx(uint8_t *data_buf, uint16_t data_size)
{
    while (data_size > 0)
    {
        while(!(SPI2->SR & SPI_SR_TXE));
        *(__IO uint8_t *)&SPI2->DR = 0xFF;

        while (!(SPI2->SR  & SPI_SR_RXNE));
        while(SPI2->SR & SPI_SR_BSY);
        *data_buf++ = *(__IO uint8_t *)&SPI2->DR;
        data_size--;
    }
}

void SPI2_SetSpeedLow(void)
{
    SPI2->CR1 &= ~SPI_CR1_SPE; // Disable SPI
    SPI2->CR1 |= SPI_CR1_BR_0 | SPI_CR1_BR_1 | SPI_CR1_BR_2; // Clock prescaler 256 (~330 kB/s) smallest possible speed
    SPI2->CR1 |= SPI_CR1_SPE; // Enable SPI
}

void SPI2_SetSpeedHigh(void)
{
    SPI2->CR1 &= ~(SPI_CR1_SPE | SPI_CR1_BR_0 | SPI_CR1_BR_1 | SPI_CR1_BR_2); // Disable SPI and clear prescale bits;
    SPI2->CR1 |= SPI_CR1_BR_0; // Clock prescaler 16 (5,5 MB/s) shoudl work witch mose devices;
    SPI2->CR1 |= SPI_CR1_SPE; // Enable SPI   

    //     SPI2->CR1 &= ~SPI_CR1_SPE; // Disable SPI
    // SPI2->CR1 |= SPI_CR1_BR_0 | SPI_CR1_BR_1 | SPI_CR1_BR_2; // Clock prescaler 256 (~330 kB/s) smallest possible speed
    // SPI2->CR1 |= SPI_CR1_SPE; // Enable SPI
}