#ifndef _AIC3204_H_
#define _AIC3204_H_
#include "sys.h"

#define AIC3204_I2C_ADDR 0x30
#define I2S_Data_Buffer_Size 1024



uint8_t AIC3204_Check(void);
void AIC3204_Init(void);




extern uint16_t I2S_ADC_Buffer[2][I2S_Data_Buffer_Size];
extern uint16_t I2S_DAC_Buffer[2][I2S_Data_Buffer_Size];


#endif
