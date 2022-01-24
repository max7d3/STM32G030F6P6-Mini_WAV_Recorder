/*
*   I made this for F0 some long time ago. It's seems F0 and G0 I2C are the same so
*   I used it here too. It's not super well made but works and I'm too lazy to modify it 
*/

#include "stm32g0xx.h"
#include "gpio.h"
#include "i2c.h"




void I2C2_Init(I2C_mode_TypeDef I2C_mode)
{

    //RCC Settings
    RCC->APBENR1 |= RCC_APBENR1_I2C2EN;

    //GPIOA Settings
    GPIO_Config(GPIO_PORTA, GPIO_PIN12, GPIO_MODE_ALTERNATE_OPEN_DRAIN_PULLUP, GPIO_SPEED_VERY_HIGH);
    GPIO_Config(GPIO_PORTA, GPIO_PIN11, GPIO_MODE_ALTERNATE_OPEN_DRAIN_PULLUP, GPIO_SPEED_VERY_HIGH);
    GPIOA->AFR[1] |= (6 << GPIO_AFRH_AFSEL12_Pos) | (6 << GPIO_AFRH_AFSEL11_Pos);


    //I2C Settings
    switch (I2C_mode)
    {
    case Standard:
        I2C2->TIMINGR = 0x10707DBC;
        break;

    case Fast:
        I2C2->TIMINGR = 0x00602173;
        break;

    case FastPlus:
        I2C2->TIMINGR = 0x00300B29;
        break;
    
    default:
        break;
    }
    
    I2C2->CR1 |= I2C_CR1_PE | I2C_CR1_TCIE | I2C_CR1_TXIE; // Enable I2C
    I2C2_IRQ_Tx_End = 1;
    I2C2_Rx_End = 1;
    __NVIC_DisableIRQ(I2C2_IRQn);

}



uint8_t I2C2_Write(uint8_t slave_adress, uint8_t register_adress, uint8_t *data, uint16_t data_size)
{
    //while((I2C2->ISR & I2C_ISR_BUSY));

    I2C2->CR2 = (slave_adress<<1) ; // Write slave adress to register I2C_CR2_RD_WRN
    I2C2->CR2 &= ~I2C_CR2_RD_WRN;

    if(data_size <= 254) // 254 bytes from buffer + register adress = 255 max
    {
        uint32_t temp = I2C2->CR2; // Copy register 
        I2C2->CR2 = ((data_size+1)<<I2C_CR2_NBYTES_Pos); // Set number of bytes to transfer (MAX 255)
        I2C2->CR2 |= temp; // Logical add register copy and register witch NBYTES set
        I2C2->CR2 |= I2C_CR2_START; // Send start condition

        for (uint8_t i = 0; i < data_size+1; i++)
        {
            if (i == 0)
            {
                 I2C2->TXDR = register_adress; // If it's first byte write adress of slave register
            }
            else
            {
                I2C2->TXDR = data[i-1]; // Write data to be sent to register
            }

            uint32_t timeout = TIMEOUT;

            while(!(I2C2->ISR & I2C_ISR_TXE)) // Wait for data to be transmitted
            {
                timeout--;


                if(timeout < 1)
                {
                   return 0;
                } 
            }
        }
        
        I2C2->CR2 |= I2C_CR2_STOP; // Send stop condition (AUTOEND doesn't work. Find out why maybe sometime) 
        //Missing setting is not a problem deleted it to save memory if it's doesn't work anyway

    }

    else
    {
        uint16_t nbytes = data_size + 1; // Number of bytes to send data buffer + slave register adress
         
         I2C2->CR2 |= (255<<I2C_CR2_NBYTES_Pos); // Datasheet
         I2C2->CR2 &= ~I2C_CR2_AUTOEND; // Turn off auto STOP generation
         I2C2->CR2 |= I2C_CR2_RELOAD; // Datasheet
         I2C2->CR2 |= I2C_CR2_START;  // Send start condition

        for (uint16_t i = 0; i < data_size + 1; i++)
        {
            if (i == 0)
            {
                 I2C2->TXDR = register_adress; // If it's first byte write adress of slave register
            }
            else
            {
                I2C2->TXDR = data[i-1]; // Write data to be sent to register
            }

            uint32_t timeout = TIMEOUT;

            while(!(I2C2->ISR & I2C_ISR_TXE))
            {
                timeout--;

                if(I2C2->ISR & I2C_ISR_TCR)
                {
                    break;
                }

                if(timeout < 1)
                {
                   return 0;
                } 
            }

            if(I2C2->ISR & I2C_ISR_TCR)
            {
                if(nbytes < 256)
                {
                    uint32_t temp = I2C2->CR2;
                    I2C2->CR2 = (nbytes<<I2C_CR2_NBYTES_Pos);
                    I2C2->CR2 |= temp;
                    I2C2->CR2 &= ~I2C_CR2_RELOAD;
                    I2C2->CR2 |= I2C_CR2_AUTOEND;
                    i--; // without it one byte it's lost
                }
                else
                {
                   I2C2->CR2 |= I2C_CR2_NBYTES;
                   nbytes = nbytes - 255; 
                   i--; // without it one byte it's lost
                }
            }
        }

        I2C2->CR2 &= ~I2C_CR2_NBYTES; // Reset NBYTES to send
        // Here somehow AUTOEND is working
    }

    return 1;

}

// uint8_t I2C_Write16Bit(uint8_t slave_adress, uint16_t register_adress, uint8_t *data, uint16_t data_size)
// {
//     I2C1->CR2 = (slave_adress<<1) ; // Write slave adress to register I2C_CR2_RD_WRN
//     I2C1->CR2 &= ~I2C_CR2_RD_WRN;

//     if(data_size <= 253) // 254 bytes from buffer + register adress = 255 max
//     {
//         uint32_t temp = I2C1->CR2; // Copy register 
//         I2C1->CR2 = ((data_size+2)<<I2C_CR2_NBYTES_Pos); // Set number of bytes to transfer (MAX 255)
//         I2C1->CR2 |= temp; // Logical add register copy and register witch NBYTES set
//         I2C1->CR2 |= I2C_CR2_START; // Send start condition

//         for (uint8_t i = 0; i < data_size+2; i++)
//         {
//             if (i == 0 || i == 1)
//             {
//                 if (i == 0)
//                 {
//                     I2C1->TXDR = (register_adress >> 8);
//                 }

//                 if (i == 1)
//                 {
//                     I2C1->TXDR = register_adress;
//                 }
                
//             }
//             else
//             {
//                 I2C1->TXDR = data[i-2]; // Write data to be sent to register
//             }

//             uint32_t timeout = TIMEOUT;

//             while(!(I2C1->ISR & I2C_ISR_TXE)) // Wait for data to be transmitted
//             {
//                 timeout--;


//                 if(timeout < 1)
//                 {
//                    return 0;
//                 } 
//             }
//         }
        
//         I2C1->CR2 |= I2C_CR2_STOP; // Send stop condition (AUTOEND doesn't work. Find out why maybe sometime) 
//         //Missing setting is not a problem deleted it to save memory if it's doesn't work anyway

//     }

//     return 1;

//}



// uint8_t I2C_Read(uint8_t slave_adress, uint8_t register_adress, uint8_t *data_output_buffer, uint16_t bytes_to_read)
// {
    
//     I2C_Write(slave_adress, register_adress,(uint8_t*)0 ,0);

//     I2C1->CR2 |= I2C_CR2_RD_WRN;

//     if (bytes_to_read <= 255)
//     {
//         uint32_t temp = I2C1->CR2; // Copy register 
//         I2C1->CR2 = ((bytes_to_read)<<I2C_CR2_NBYTES_Pos); // Set number of bytes to receive (MAX 255)
//         I2C1->CR2 |= temp; // Logical add register copy and register witch NBYTES set
//         I2C1->CR2 |= I2C_CR2_START; // Send start condition

//         uint8_t rx_done = 0;
//         uint16_t timeout = 0;
//         uint16_t rxed_bytes = 0;

//         while(!rx_done && timeout < TIMEOUT)
//         {
//             timeout++;

//              if(I2C1->ISR & I2C_ISR_RXNE)
//              {
//                 data_output_buffer[rxed_bytes] = I2C1->RXDR;
//                 rxed_bytes++;
//                 timeout = 0;
//              }

//              if (I2C1->ISR & I2C_ISR_TC) 
//              {
//                 rx_done = 1;
//                 I2C1->CR2 |= I2C_CR2_STOP;
//                 return 1;
//              }

//         }

//     }
//     return 0;
// }

// uint8_t I2C_Read16Bit(uint8_t slave_adress, uint16_t register_adress, uint8_t *data_output_buffer, uint16_t bytes_to_read)
// {

//     //I2C_Rx_End = 0; 

//     I2C_Write16Bit(slave_adress, register_adress,(uint8_t*)0 ,0);

//     I2C1->CR2 |= I2C_CR2_RD_WRN;

//     if (bytes_to_read <= 255)
//     {
//         uint32_t temp = I2C1->CR2; // Copy register 
//         I2C1->CR2 = ((bytes_to_read)<<I2C_CR2_NBYTES_Pos); // Set number of bytes to receive (MAX 255)
//         I2C1->CR2 |= temp; // Logical add register copy and register witch NBYTES set
//         I2C1->CR2 |= I2C_CR2_START; // Send start condition

//         uint8_t rx_done = 0;
//         uint16_t timeout = 0;
//         uint16_t rxed_bytes = 0;

//         while(!rx_done && timeout < TIMEOUT)
//         {
//             timeout++;

//              if (I2C1->ISR & I2C_ISR_TC) 
//              {
//                 rx_done = 1;
//                 I2C1->CR2 |= I2C_CR2_STOP;
//                 //I2C_Rx_End = 1;
//                 return 1;
//              }

//              if(I2C1->ISR & I2C_ISR_RXNE)
//              {
//                 data_output_buffer[rxed_bytes] = I2C1->RXDR;
//                 rxed_bytes++;
//                 timeout = 0;
//              }

//         }

//     }
//     return 0;
// }


void I2C2_Write_IRQ(uint8_t slave_adress, uint8_t register_adress, uint8_t *data, uint16_t data_size)
{
    if(I2C2_IRQ_Tx_End)
    {
        __NVIC_EnableIRQ(I2C2_IRQn);
        I2C2->CR2 = (slave_adress<<1); // Write slave adress to register
        I2C2->CR2 &= ~I2C_CR2_RD_WRN;

        I2C2_DataPointer = data;
        I2C2_RegisterAdr = register_adress;

        if(data_size <= 254) // 254 bytes from buffer + register adress = 255 max
        {
            uint32_t temp = I2C2->CR2; // Copy register 
            I2C2->CR2 = ((data_size+1)<<I2C_CR2_NBYTES_Pos); // Set number of bytes to transfer (MAX 255)
            I2C2->CR2 |= temp; // Logical add register copy and register witch NBYTES set
            I2C2->CR2 |= I2C_CR2_START; // Send start condition

            I2C2->TXDR = register_adress;
            I2C2_IRQ_Tx_End = 0;
        }
        else
        {
            I2C2_Nbytes = (data_size + 1); // Number of bytes to send data buffer + slave register adress
         
            I2C2->CR2 |= (255<<I2C_CR2_NBYTES_Pos); // Datasheet
            I2C2->CR2 &= ~I2C_CR2_AUTOEND; // Turn off auto STOP generation
            I2C2->CR2 |= I2C_CR2_RELOAD; // Datasheet
            I2C2->CR2 |= I2C_CR2_START;  // Send start condition     

            I2C2->TXDR = register_adress;
            I2C2_IRQ_Tx_End = 0;       
        }
        
    }    
}

void I2C2_IRQWait (void)
{
    while(!I2C2_IRQ_Tx_End);
}

void I2C2_IRQHandler(void)
{
    uint32_t I2C2_IRQ_status = I2C2->ISR;

    if (I2C2_IRQ_status & I2C_ISR_TC)
    {
        I2C2->CR2 |= I2C_CR2_STOP;
        I2C2->CR2 &= ~I2C_CR2_NBYTES;
        I2C2_IRQ_Tx_End = 1;
        __NVIC_DisableIRQ(I2C2_IRQn);
        return;
    }

    if(I2C2_IRQ_status & I2C_ISR_TCR)
    {
        if(I2C2_Nbytes < 256)
        {
            uint32_t temp = I2C2->CR2;
            I2C2->CR2 = (I2C2_Nbytes<<I2C_CR2_NBYTES_Pos);
            I2C2->CR2 |= temp;
            I2C2->CR2 &= ~I2C_CR2_RELOAD;
            //I2C1->CR2 |= I2C_CR2_AUTOEND;
        }
        else
        {
            I2C2->CR2 |= I2C_CR2_NBYTES | I2C_CR2_RELOAD;
            I2C2_Nbytes = I2C2_Nbytes - 255; 
        }        
    }

    if (I2C2_IRQ_status & I2C_ISR_TXIS)
    {
        I2C2->TXDR = *I2C2_DataPointer;
        I2C2_DataPointer++;
    }


}