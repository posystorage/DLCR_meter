#include "sys.h" 
#include "LCD.h"
#include "I2C.h"
#include "AIC3204.h"
#include "EEPROM24C.h"
#include "LCR_core.h"

void I2C_Peripheral_Check(void)
{
	uint8_t Error_Code;
	POINT_COLOR=YELLOW;
	Error_Code=EEPROM_24C02_Init();
	if(Error_Code)
	{
		if(Error_Code==1)LCD_ShowStr(10,10,"No EEPROM Found",24);
		if(Error_Code==2)LCD_ShowStr(10,10,"EEPROM Write Error",24);
		if(Error_Code==3)LCD_ShowStr(10,10,"EEPROM CheckSum Error",24);
		while(1);
	}
	delay_us(20);
	if(AIC3204_Check())
	{
		LCD_ShowStr(10,10,"No AIC3204 Found",24);
		while(1);
	}
}	


void Sys_Init(void)
{ 
	RCC->AHB1ENR|=RCC_AHB1ENR_GPIOAEN|RCC_AHB1ENR_GPIOBEN|RCC_AHB1ENR_GPIOCEN;    //Ê¹ÄÜPORTA B CÊ±ÖÓ 
	SCB->AIRCR = 0X05FA0000|(4<<8);//Group priorities=8  Sub priorities=2
	uart1_init(SystemCoreClock,115200);
	delay_init(96);
	LCD_Init();
	I2C_Init();
	I2C_Peripheral_Check();
	AIC3204_Init();
	

//	
//	delay_us(100);
//	if(I2C_Check_Slave_Device(0x30)==1)
//	{
//		LCD_Fill(100,10,150,20,RED);
//	}	
//	
//	LCD_ShowChar(10,100,'D',48);
//	
//	
//	LCD_ShowChar(60,50,'B',24);
//	LCD_ShowChar(100,50,'C',32);
}



int main(void)
{
	Sys_Init();	
	while(1)
	{

	}
}

