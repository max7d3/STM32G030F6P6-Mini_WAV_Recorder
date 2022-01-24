#ifndef _I2S_H_
#define _I2S_H_

void I2S_Init(void);
void I2S_Read(uint16_t *output, uint16_t data_size);
void I2S_StartDMA(uint16_t *output, uint16_t data_size);

#endif /* _I2S_H_ */