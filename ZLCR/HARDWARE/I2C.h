#ifndef __I2C_H
#define __I2C_H	 
#include "sys.h"

	

void I2C_Init(void);	//≥ı ºªØ		
uint32_t I2C_Check_Slave_Device(uint8_t I2C_Addr);

void I2C_Write_Reg(uint8_t I2C_Addr,uint8_t Reg_Addr,uint8_t Data);
uint8_t I2C_Read_Reg(uint8_t I2C_Addr,uint8_t Reg_Addr);
void I2C_Write_NBytes(uint8_t I2C_Addr,uint8_t Reg_Addr,uint8_t* Data,uint32_t Length);
void I2C_Read_NBytes(uint8_t I2C_Addr,uint8_t Reg_Addr,uint8_t* Data,uint32_t Length);

#endif
