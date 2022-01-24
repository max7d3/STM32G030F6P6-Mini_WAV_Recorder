#ifndef _SPI_H_
#define _SPI_H_

#define SPI2_SD_CS_HIGH {GPIOA->BSRR = GPIO_BSRR_BS8;}
#define SPI2_SD_CS_LOW {GPIOA->BSRR = GPIO_BSRR_BR8;}

void SPI2_Init(void);

uint8_t SPI2_ExchangeByte(uint8_t tx_byte);

void SPI2_Tx(uint8_t *data, uint16_t data_size);

void SPI2_Rx(uint8_t *data_buf, uint16_t data_size);

void SPI2_SetSpeedLow(void);

void SPI2_SetSpeedHigh(void);

#endif /* _SPI_H_ */