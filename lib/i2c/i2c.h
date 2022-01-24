/*
    I2C Library for STM32F0 v0.1
    
*/

#ifndef F0_I2C_H_
#define F0_I2C_H_

#define TIMEOUT 1000 // 100 at 48 MHz is minimum 

typedef enum
{
    Standard,
    Fast,
    FastPlus
    
} I2C_mode_TypeDef;

I2C_mode_TypeDef I2C_mode;

volatile uint8_t *I2C2_DataPointer;
volatile uint8_t I2C2_RegisterAdr;
volatile uint16_t I2C2_Nbytes;
volatile uint8_t I2C2_IRQ_Tx_End;

uint8_t I2C2_Rx_End;



void I2C2_Init(I2C_mode_TypeDef I2C_mode);

//Write to I2C device 
uint8_t I2C2_Write(uint8_t slave_adress, uint8_t register_adress, uint8_t *data, uint16_t data_size);

// //Write to I2C device using 16 bit registers (actually 255 bytes max)
// uint8_t I2C_Write16Bit(uint8_t slave_adress, uint16_t register_adress, uint8_t *data, uint16_t data_size);

// //Read from I2C device (actually 255 bytes max)
// uint8_t I2C_Read(uint8_t slave_adress, uint8_t register_adress, uint8_t *data_output_buffer, uint16_t bytes_to_read);

// //Read from I2C device using 16 bit registers (actually 255 bytes max)
// uint8_t I2C_Read16Bit(uint8_t slave_adress, uint16_t register_adress, uint8_t *data_output_buffer, uint16_t bytes_to_read);

void I2C2_Write_IRQ(uint8_t slave_adress, uint8_t register_adress, uint8_t *data, uint16_t data_size);


void I2C2_IRQWait (void); // If using intterups wait until data transfer is complete to avoid data corruption






#endif /* F0_I2C_H_ */