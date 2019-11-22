#include "EEPROM24C.h"
#include "I2C.h"

uint8_t EEP_Buff[0x100];

//err code:
//0-ok
//1-iic_sla_nask
//2-addr:0xff=0x5a check err
//3-data checksum err
uint8_t EEPROM_24C02_Init(void)
{
	if(!I2C_Check_Slave_Device(EEPROM24C02_I2C_ADDR))
	{
		return 1;
	}

	if(I2C_Read_Reg(EEPROM24C02_I2C_ADDR,0xFF)!=0x5A)
	{
		I2C_Write_Reg(EEPROM24C02_I2C_ADDR,0xFF,0x5A);
		delay_ms(5);
		if(I2C_Read_Reg(EEPROM24C02_I2C_ADDR,0xFF)!=0x5A)
		{
			return 2;
		}
	}
	I2C_Read_NBytes(EEPROM24C02_I2C_ADDR,0,EEP_Buff,0x100);
	return 0;
}

