#include "LCR_core.h"
#include "LCR_data.h" 

//IIR filter
arm_biquad_casd_df1_inst_f32 ZLCR_prv_iir_inst   [8];
float           ZLCR_prv_iir_state  [8][32];

void LCR_Init(void)
{



}

void LCR_Set_Ftrq(float *freq)
{


}



float LCR_Get_Ftrq(void)
{

	return 0;
}

void LCR_Call_Back(uint16_t* ADC_Data_Buffer_Addr,uint16_t* DAC_Data_Buffer_Addr,uint32_t Data_Size)
{



}


void LCR_Data_Service(void)
{


}
