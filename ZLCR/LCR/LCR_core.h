#ifndef _LCR_CORE_H_
#define _LCR_CORE_H_
#include "sys.h" 

void LCR_Init(void);
void LCR_Data_Init_Call_Back(uint16_t* DAC_Data_Buffer_Addr,uint32_t Data_Size);
void LCR_Set_Ftrq(float *freq);
float LCR_Get_Ftrq(void);
void LCR_Call_Back(uint16_t* ADC_Data_Buffer_Addr,uint16_t* DAC_Data_Buffer_Addr,uint32_t Data_Size);
void LCR_Data_Service(void);








#endif
