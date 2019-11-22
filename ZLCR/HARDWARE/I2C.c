#include "I2C.h"
//PB8 AF4-I2C1_SCL
//PB9 AF4-I2C1_SDA

void I2C_Init(void)
{
	RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

	GPIOB->OTYPER |= GPIO_Pin_8|GPIO_Pin_9;//OC
	GPIOB->MODER&=~0x000F0000;//B8/9
	GPIOB->MODER|= 0x000A0000;
	GPIOB->AFR[1]&=~0x000000FF;
	GPIOB->AFR[1]|= 0x00000044;		

	I2C1->CR1=I2C_CR1_SWRST;
	I2C1->CR1=0;
	I2C1->CR2=48;//APB1 48MHz
	I2C1->CCR=I2C_CCR_FS|I2C_CCR_DUTY|5;//Fm mode 9:16 384KHz
	I2C1->TRISE= 5;
	
	I2C1->CR1=I2C_CR1_PE;
}

uint32_t I2C_Check_Slave_Device(uint8_t I2C_Addr)
{
	uint32_t Time_Out=1000;
	uint32_t Return_code;
	uint16_t I2C_SR1;
	if((I2C1->SR2&I2C_SR2_BUSY) == 0)
	{
		I2C1->CR1 |= I2C_CR1_START;
		while((I2C1->SR1&I2C_SR1_SB) == 0);
		I2C1->DR = I2C_Addr;
		//Return_code=1;
		while(1)
		{
			I2C_SR1=I2C1->SR1;
			I2C1->SR2;
			if((I2C_SR1&I2C_SR1_ADDR) == I2C_SR1_ADDR)//ASK
			{
				Return_code=1;
				break;
			}
			if((I2C_SR1&I2C_SR1_AF) == I2C_SR1_AF)//NASK
			{
				I2C1->SR1=0;
				Return_code=0;
				break;
			}	
			Time_Out--;
			if(Time_Out==0)
			{
				Return_code=0;
				break;			
			}
		}
		I2C1->CR1 |= I2C_CR1_STOP;
		while((I2C1->SR2&I2C_SR2_BUSY) == I2C_SR2_BUSY);	
	}
	else
		Return_code=0;	
	return Return_code;
}

void I2C_Write_Reg(uint8_t I2C_Addr,uint8_t Reg_Addr,uint8_t Data)
{
	while((I2C1->SR2&I2C_SR2_BUSY) == I2C_SR2_BUSY);

	I2C1->CR1 |= I2C_CR1_START;
	while((I2C1->SR1&I2C_SR1_SB) == 0);
	I2C1->DR = I2C_Addr;	
	while((I2C1->SR1&I2C_SR1_ADDR) == 0);
	I2C1->SR2;
	
	I2C1->DR = Reg_Addr;	
	while((I2C1->SR1&(I2C_SR1_TXE|I2C_SR1_BTF)) != (I2C_SR1_TXE|I2C_SR1_BTF));
	I2C1->DR = Data;	
	while((I2C1->SR1&(I2C_SR1_TXE|I2C_SR1_BTF)) != (I2C_SR1_TXE|I2C_SR1_BTF));
	I2C1->CR1 |= I2C_CR1_STOP;			
}

void I2C_Write_NBytes(uint8_t I2C_Addr,uint8_t Reg_Addr,uint8_t* Data,uint32_t Length)
{
	while((I2C1->SR2&I2C_SR2_BUSY) == I2C_SR2_BUSY);

	I2C1->CR1 |= I2C_CR1_START;
	while((I2C1->SR1&I2C_SR1_SB) == 0);
	I2C1->DR = I2C_Addr;	
	while((I2C1->SR1&I2C_SR1_ADDR) == 0);
	I2C1->SR2;
	
	I2C1->DR = Reg_Addr;	
	while((I2C1->SR1&(I2C_SR1_TXE|I2C_SR1_BTF)) != (I2C_SR1_TXE|I2C_SR1_BTF));
	
	while(Length--)
	{
		while((I2C1->SR1&I2C_SR1_TXE) != I2C_SR1_TXE);
		I2C1->DR = *Data;		
		Data++;		
	}
	while((I2C1->SR1&(I2C_SR1_TXE|I2C_SR1_BTF)) != (I2C_SR1_TXE|I2C_SR1_BTF));
	I2C1->CR1 |= I2C_CR1_STOP;			
}

void I2C_Read_Head(uint8_t I2C_Addr,uint8_t Reg_Addr)
{
	while((I2C1->SR2&I2C_SR2_BUSY) == I2C_SR2_BUSY);
	I2C1->CR1 |= I2C_CR1_START;
	while((I2C1->SR1&I2C_SR1_SB) == 0);
	I2C1->DR = I2C_Addr;	
	while((I2C1->SR1&I2C_SR1_ADDR) == 0);
	I2C1->SR2;	
	I2C1->DR = Reg_Addr;	
	while((I2C1->SR1&(I2C_SR1_TXE|I2C_SR1_BTF)) != (I2C_SR1_TXE|I2C_SR1_BTF));
	
	I2C1->CR1 |= I2C_CR1_START;
	while((I2C1->SR1&I2C_SR1_SB) == 0);
	I2C1->DR = I2C_Addr|0x01;	
	while((I2C1->SR1&I2C_SR1_ADDR) == 0);
	I2C1->SR2;
}

uint8_t I2C_Read_Reg(uint8_t I2C_Addr,uint8_t Reg_Addr)
{
	I2C_Read_Head(I2C_Addr,Reg_Addr);
	
	I2C1->CR1 &=~I2C_CR1_ACK;
	I2C1->CR1 |= I2C_CR1_STOP;
	while((I2C1->SR1&I2C_SR1_RXNE) == 0);	
	return I2C1->DR;
}

void I2C_Read_NBytes(uint8_t I2C_Addr,uint8_t Reg_Addr,uint8_t* Data,uint32_t Length)
{
	I2C_Read_Head(I2C_Addr,Reg_Addr);
	I2C1->CR1 |= I2C_CR1_ACK;
	while(Length--)
	{
		if(Length==0)
		{
			I2C1->CR1 &=~I2C_CR1_ACK;
			I2C1->CR1 |= I2C_CR1_STOP;
		}
		while((I2C1->SR1&I2C_SR1_RXNE) == 0);	
		*Data=I2C1->DR;
		Data++;
	}
}

