#include "AIC3204.h"
#include "I2C.h"
#include "LCR_core.h"
//Data format=16bit
//HSI 12MHz
//PLLM=12
//MCU_I2S_PLLIN=1MHz
//Fs = I2SxCLK / [(16*2)*((2*I2SDIV)+ODD))] 

//192KHz 
//MCU_PLLI2S_N=424
//MCU_PLLI2S_R=3
//MCU_PLL_OUT=141.333333333MHz
//I2SDIV=11 ODD=1-->I2SDIV=23
//Fs=192.0289844KHz

//200KHz
//MCU_PLLI2S_N=320
//MCU_PLLI2S_R=2
//MCU_PLL_OUT=160MHz
//I2SDIV=12 ODD=1-->I2SDIV=25
//Fs=200KHz

//202KHz
//MCU_PLLI2S_N=362
//MCU_PLLI2S_R=7
//MCU_PLL_OUT=160MHz
//I2SDIV=4 ODD=0-->I2SDIV=8
//Fs=202.009KHz

//204KHz
//MCU_PLLI2S_N=235
//MCU_PLLI2S_R=6
//MCU_PLL_OUT=160MHz
//I2SDIV=3 ODD=0-->I2SDIV=6
//Fs=203.993KHz

///AIC3204 CODIC PLL
//PLL_CLK_OUT=PLL_CLK_IN*R*J.D/P
//PLL_CLK_IN/P<=20MHz D=0:PLL_CLK_IN/P>=512KHz D=1:PLL_CLK_IN/P>=10MHz
//PLL_CLK_IN=Fs*32 --eg:192K->6.122MHz
//D=0 P=1
//J=8 R=1

#define PLLI2S_R 3
#define PLLI2S_N 424
#define I2S_DIV 11
#define I2S_ODD 1

uint16_t I2S_ADC_Buffer[2][I2S_Data_Buffer_Size];
uint16_t I2S_DAC_Buffer[2][I2S_Data_Buffer_Size];

const uint8_t ZLCR_AIC3204_CODEC_Init_CMD_Table[][2] = 
{
    {0x00, 0x00}, //Select Page 0
    /* PLL Step */
		{0x07, 0}, 	//PLL_D=0
		{0x08, 0},
		{0x06, 8},	//PLL_J=8
		{0x04, 7},	//Low PLL Range.BCLK pin is input to PLL.PLL Clock is CODEC_CLKIN		
		{0x05, 0x91},	//PLL ON PLL_P=1 PLL_R=1
    /* DAC Step */		
    {0x0b, 0x81}, //NDAC = 1, Power up
    {0x0c, 0x82}, //MDAC = 2, Power up
    {0x0d, 0x00}, //DOSR = 128
    {0x0e, 0x80}, //DOSR LSB
    {0x3c, 0x01}, //PRB_P1

    /* ADC Step */
    {0x12, 0x81}, //NADC = 1, Power up
    {0x13, 0x82}, //MADC = 2, Power up
    {0x14, 0x80}, //AOSR = 128
    {0x3d, 0x01}, //PRB_R1

    /* Audio Interface */
    {0x1b, 0x00}, //16bit
    {0x55, 0x00}, //ADC Phase Adjust Register

    /* Power Step */
    {0x00, 0x01}, //Select Page 1
    {0x01, 0x08}, //Disabled weak connection of AVDD with DVDD
    {0x02, 0x01}, //Analog Block Power up, AVDD LDO Power up
    // {0x0a, 0x3b}, //Input Vcom = 0.9v, Output Vcom = 1.65v
    {0x0a, 0x03},

    {0x03, 0x00}, //DAC PTM mode to PTM_P3/4
    {0x04, 0x00},
    {0x3d, 0x00}, //ADC PTM mode to PTM_R4

    // {0x7b, 0x01}, //REF settime to 40ms
    // {0x14, 0x25}, //HP settime  to

    /* Input Step */
    {0x34, 0x10}, //Route IN2L to LEFT_P with 10k  //Vol
    {0x36, 0x10}, //Route IN2R to LEFT_M with 10k
    {0x37, 0x40}, //Route IN1R to RIGHT_P with 10k   //Cur
    {0x39, 0x10}, //Route IN1L to RIGHT_M with 10k

    {0x3b, 0x00}, //Left  MicPGA not mute, gain to 0dB
    {0x3c, 0x00}, //Right MicPGA not mute, gain to 0dB

    /* Output Step */
    {0x0c, 0x08}, //Route Left  DAC Positive to HPL
    {0x0d, 0x08}, //Route Right DAC Positive to HPR
    {0x0e, 0x08}, //Route Left  DAC Negtive to LOL
    {0x0f, 0x08}, //Route Right DAC Negtive to LOR

    {0x10, 0x00}, //HPL gain to 0dB
    {0x11, 0x00}, //HPR gain to 0dB
    {0x12, 0x08}, //LOL gain to 0dB
    {0x13, 0x08}, //LOR gain to 0dB

    {0x16, 0x75}, //IN1L to HPL, MUTE
    {0x17, 0x75}, //IN1R to HPL, MUTE

    {0x09, 0x3c}, //LOL, LOR, HPL, HPR, Power up

    /* Initial ok */
    {0x00, 0x00}, //Select Page 0
    {0x3f, 0xd6}, //L&R DAC Power up
    {0x40, 0x00}, //L&R DAC not mute
    {0x51, 0xc0}, //L&R ADC Power up
    {0x52, 0x00}, //L&R ADC not mute, ADC Fine Gain Adjust
    {0x53, 0x00}, //Left  ADC Digital gain
    {0x54, 0x00}, //Right ADC Digital gain
};


uint8_t AIC3204_Check(void)
{
	if(I2C_Check_Slave_Device(AIC3204_I2C_ADDR)==0)
	{
		return 1;
	}
	return 0;
}

//PB12 IIS2_WS AF5
//PB13 IIS2_SCK AF5
//PB14 IIS2_SDIN AF6!
//PB15 IIS2_SDOUT AF5
//DMA1-Steam3 CH0-SPI2_RX CH3-I2S2_EXT_RX
//DMA1-Steam4 CH0-SPI2_TX CH2-I2S2_EXT_TX
void I2S_Init(void)
{
	RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;
	//Setting PLL_I2S
	RCC->PLLI2SCFGR = (PLLI2S_R<<28)|(PLLI2S_N<<6);
	RCC->CR |= RCC_CR_PLLI2SON;
	
	GPIOB->MODER &=~0xff000000;
	GPIOB->OSPEEDR |= 0xff000000;
	GPIOB->MODER |= 0xAA000000;
	GPIOB->OTYPER &=~ 0xf000;
	GPIOB->AFR[1] &=~0xffff0000;
	GPIOB->AFR[1] |= 0x56550000;	
	while((RCC->CR & RCC_CR_PLLI2SRDY) == 0)
	{
	}
	SPI2->I2SPR = (I2S_ODD<<8)|I2S_DIV;//192Khz
	SPI2->I2SCFGR = SPI_I2SCFGR_I2SMOD|SPI_I2SCFGR_I2SCFG_1;//I2S mode,Master_TX,I2S_Philips,Clk_S_Low,16bit
	SPI2->CR2 = SPI_CR2_TXDMAEN;
	I2S2ext->I2SCFGR = SPI_I2SCFGR_I2SMOD|SPI_I2SCFGR_I2SCFG_0;//I2S mode,Slave_RX,I2S_Philips,Clk_Low,16bit
	I2S2ext->CR2 = SPI_CR2_RXDMAEN;
	I2S2ext->I2SCFGR = 2;
	
	DMA1_Stream3->PAR = (uint32_t)(&I2S2ext->CRCPR);
	DMA1_Stream3->M0AR = (uint32_t)&I2S_ADC_Buffer[0][0];
	DMA1_Stream3->M1AR = (uint32_t)&I2S_ADC_Buffer[1][0];
	DMA1_Stream3->NDTR = I2S_Data_Buffer_Size;
	DMA1_Stream3->FCR = DMA_SxFCR_DMDIS|DMA_SxFCR_FTH_0;
	DMA1_Stream3->CR = (3<<25)|DMA_SxCR_DBM|DMA_SxCR_PL_1|DMA_SxCR_MSIZE_0|DMA_SxCR_PSIZE_0|DMA_SxCR_MINC|DMA_SxCR_PFCTRL|DMA_SxCR_TCIE;
	
	DMA1_Stream4->PAR = (uint32_t)(&SPI2->CRCPR);
	DMA1_Stream4->M0AR = (uint32_t)&I2S_DAC_Buffer[0][0];
	DMA1_Stream4->M1AR = (uint32_t)&I2S_DAC_Buffer[1][0];
	DMA1_Stream4->NDTR = I2S_Data_Buffer_Size;
	DMA1_Stream4->FCR = DMA_SxFCR_DMDIS|DMA_SxFCR_FTH_0;
	DMA1_Stream4->CR = (0<<25)|DMA_SxCR_DBM|DMA_SxCR_PL_0|DMA_SxCR_MSIZE_0|DMA_SxCR_PSIZE_0|DMA_SxCR_MINC|DMA_SxCR_DIR_0|DMA_SxCR_PFCTRL;
	
	NVIC->ISER[DMA1_Stream3_IRQn/32] |= 1<<DMA1_Stream3_IRQn%32;
	NVIC->IP[DMA1_Stream3_IRQn] = (3<<5)|(0<<4);
	
	SPI2->I2SCFGR |= SPI_I2SCFGR_I2SE;
	I2S2ext->I2SCFGR |= SPI_I2SCFGR_I2SE;	
	DMA1_Stream3->CR |= DMA_SxCR_EN;
	DMA1_Stream4->CR |= DMA_SxCR_EN;
}//DMA1_Stream3_IRQn

void DMA1_Stream3_IRQHandler(void)
{
	if((DMA1->LISR&DMA_LISR_TCIF3)==DMA_LISR_TCIF3)
	{
		if((DMA1_Stream3->CR&DMA_SxCR_CT)==DMA_SxCR_CT)
		{
			LCR_Call_Back(&I2S_ADC_Buffer[0][0],&I2S_DAC_Buffer[0][0],I2S_Data_Buffer_Size);
		}
		else
		{
			LCR_Call_Back(&I2S_ADC_Buffer[1][0],&I2S_DAC_Buffer[1][0],I2S_Data_Buffer_Size);		
		}	
	}
	DMA1->LIFCR |= DMA_LIFCR_CTCIF3|DMA_LIFCR_CHTIF3|DMA_LIFCR_CTEIF3|DMA_LIFCR_CDMEIF3|DMA_LIFCR_CFEIF3;
	//DMA1->HIFCR |= DMA_HISR_TCIF4;
}


void AIC3204_Init(void)
{
	uint32_t i;
	uint32_t Cmd_Table_Size=sizeof(ZLCR_AIC3204_CODEC_Init_CMD_Table)/2;
	
	I2S_Init();

	//TLV320AIC3204 reset
	I2C_Write_Reg(AIC3204_I2C_ADDR,0x00,0x00);//page0
	I2C_Write_Reg(AIC3204_I2C_ADDR,0x01,0x01);
	delay_ms(1);    
	
	// PLL Step	
//	I2C_Write_Reg(AIC3204_I2C_ADDR,0x07,0);//PLL_D=0
//	I2C_Write_Reg(AIC3204_I2C_ADDR,0x08,0);
//	I2C_Write_Reg(AIC3204_I2C_ADDR,0x06,8);//PLL_J=8
//	I2C_Write_Reg(AIC3204_I2C_ADDR,0x04,7);//Low PLL Range.BCLK pin is input to PLL.PLL Clock is CODEC_CLKIN
//	I2C_Write_Reg(AIC3204_I2C_ADDR,0x05,0x91);//PLL ON PLL_P=1 PLL_R=1
//	delay_ms(10);
	
	for(i=0;i<Cmd_Table_Size;i++)
	{
		I2C_Write_Reg(AIC3204_I2C_ADDR,ZLCR_AIC3204_CODEC_Init_CMD_Table[i][0],ZLCR_AIC3204_CODEC_Init_CMD_Table[i][1]);
	}
}
