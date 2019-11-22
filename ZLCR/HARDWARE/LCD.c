#include "LCD.h"
#include "font.h"
//LCD 引脚定义
//LCD_D0~D7 PB0~PB7
//LCD_CS PB10
//LCD_WR PA3 TIM2_CH4
//LCD_RS PA15 
//LCD_BK PA8 TIM1_CH1 

uint16_t POINT_COLOR;
uint16_t BACK_COLOR;

////////////////////////////////////////////////////////////////////
//-----------------LCD端口定义---------------- 

#define	LCD_CS_SET  GPIOB->BSRRL=1<<10    //片选端口
#define	LCD_RS_SET	GPIOA->BSRRL=1<<15    //数据/命令 
#define	LCD_WR_SET	GPIOA->BSRRL=1<<3    //写数据

#define	LCD_CS_CLR  GPIOB->BSRRH=1<<10     //片选端口
#define	LCD_RS_CLR	GPIOA->BSRRH=1<<15     //数据/命令	   
#define	LCD_WR_CLR	GPIOA->BSRRH=1<<3     //写数据

#define DATAOUT(x) *(__IO uint32_t*)(GPIOB_BASE+0x18)=0x00FF0000|x;//数据输出
#define LCD_Quick_WR_DATA(x) DATAOUT(x);LCD_WR_CLR;LCD_WR_SET;
////////////////////////////////////////////////////////////////////// 


//写寄存器函数
//data:寄存器值
void LCD_WR_REG(uint8_t data)
{
		LCD_CS_CLR;
	  LCD_RS_CLR;//写地址
    DATAOUT(data);
    LCD_WR_CLR;
    LCD_WR_SET;	
    LCD_CS_SET;
  	//LCD_RS_SET;
}
//写数据函数
//可以替代LCD_WR_DATAX宏,拿时间换空间.
//data:寄存器值
void LCD_WR_DATA(uint8_t data)
{
    LCD_CS_CLR;
  	LCD_RS_SET;
    DATAOUT(data);
    LCD_WR_CLR;
    LCD_WR_SET;	
    LCD_CS_SET;
}



//写寄存器
//LCD_Reg:寄存器编号
//LCD_RegValue:要写入的值
void LCD_WriteReg(uint8_t LCD_Reg,uint8_t LCD_RegValue)
{
    LCD_WR_REG(LCD_Reg);
    LCD_WR_DATA(LCD_RegValue);
}


//清屏函数
//color:要清屏的填充色
#if 0
void LCD_Clear(u16 color)
{
    u32 index=0;  
		uint8_t Data_h,Data_l;
	
		Data_h=(color>>8)&0xff;
		Data_l=color&0xff;
	
	  LCD_Set_Cursor(0x00,0x00);   	 //设置光标位
    LCD_WriteRAM_Prepare();					 //开始写入GRAM
		LCD_CS_CLR;
		LCD_RS_SET;
    for(index=0; index<240*240; index++)//240*240
    {
			DATAOUT(Data_h);
			LCD_WR_CLR;
			LCD_WR_SET;	
			DATAOUT(Data_l);
			LCD_WR_CLR;
			LCD_WR_SET;				
    }
		LCD_CS_SET;
}
#else
void LCD_Clear(u16 color)
{
    u32 index=0; 
		uint32_t color_h=0x00FF0000|((color>>8)&0xff);
		uint32_t color_l=0x00FF0000|(color&0xff);
	  LCD_Set_Cursor(0x00,0x0000);   	 //设置光标位
		LCD_CS_CLR;
		LCD_RS_SET;
    for(index=0; index<240*240/4; index++)//240*432
    {
			*(__IO uint32_t*)(GPIOB_BASE+0x18)=color_h;LCD_WR_CLR;LCD_WR_SET;	
			*(__IO uint32_t*)(GPIOB_BASE+0x18)=color_l;LCD_WR_CLR;LCD_WR_SET;	
			*(__IO uint32_t*)(GPIOB_BASE+0x18)=color_h;LCD_WR_CLR;LCD_WR_SET;	
			*(__IO uint32_t*)(GPIOB_BASE+0x18)=color_l;LCD_WR_CLR;LCD_WR_SET;				
			*(__IO uint32_t*)(GPIOB_BASE+0x18)=color_h;LCD_WR_CLR;LCD_WR_SET;	
			*(__IO uint32_t*)(GPIOB_BASE+0x18)=color_l;LCD_WR_CLR;LCD_WR_SET;				
			*(__IO uint32_t*)(GPIOB_BASE+0x18)=color_h;LCD_WR_CLR;LCD_WR_SET;	
			*(__IO uint32_t*)(GPIOB_BASE+0x18)=color_l;LCD_WR_CLR;LCD_WR_SET;				
    }
		LCD_CS_SET;
}
#endif


void LCD_HW_Init(void)
{
	GPIOB->BSRRL=GPIO_Pin_10;
	GPIOB->OTYPER&=~(GPIO_Pin_10|0x000000FF);		
	GPIOB->MODER&=~0x0030FFFF;//B10 B0-7
	GPIOB->MODER|= 0x00105555;
	GPIOB->OSPEEDR|=0x0030FFFF;
	
	GPIOA->BSRRL=GPIO_Pin_3|GPIO_Pin_15;
	GPIOA->OTYPER&=~(GPIO_Pin_3|GPIO_Pin_15);	
	GPIOA->MODER&=~0x000000C0;//PA3 PA15
	GPIOA->OSPEEDR|=0x000000C0;
	GPIOA->MODER&=~0xC0000000;
	GPIOA->OSPEEDR|=0xC0000000;
	GPIOA->MODER|= 0x00000040;
	GPIOA->MODER|= 0x40000000;		
}

void LCD_CMD_Init(void)
{
	delay_ms(10);
	LCD_WR_REG(0x11);
	delay_ms(10);
	
	//--------------------------------Display and color format setting-------------------

	LCD_WR_REG(0x36);
	LCD_WR_DATA(0x00);
	LCD_WR_REG(0x3a);
	LCD_WR_DATA(0x05);
	//--------------------------------ST7789S Frame rate setting-------------------------

	LCD_WR_REG(0xb2);
	LCD_WR_DATA(0x0c);
	LCD_WR_DATA(0x0c);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x33);
	LCD_WR_DATA(0x33);
	LCD_WR_REG(0xb7);
	LCD_WR_DATA(0x35);
	//---------------------------------ST7789S Power setting-----------------------------

	LCD_WR_REG(0xbb);
	LCD_WR_DATA(0x35);
	LCD_WR_REG(0xc0);
	LCD_WR_DATA(0x2c);
	LCD_WR_REG(0xc2);
	LCD_WR_DATA(0x01);
	LCD_WR_REG(0xc3);
	LCD_WR_DATA(0x13);
	LCD_WR_REG(0xc4);
	LCD_WR_DATA(0x20);
	LCD_WR_REG(0xc6);
	LCD_WR_DATA(0x0f);
	LCD_WR_REG(0xca);
	LCD_WR_DATA(0x0f);
	LCD_WR_REG(0xc8);
	LCD_WR_DATA(0x08);
	LCD_WR_REG(0x55);
	LCD_WR_DATA(0x90);
	LCD_WR_REG(0xd0);
	LCD_WR_DATA(0xa4);
	LCD_WR_DATA(0xa1);
	//--------------------------------ST7789S gamma setting------------------------------
	LCD_WR_REG(0xe0);
	LCD_WR_DATA(0x70);
	LCD_WR_DATA(0x04);
	LCD_WR_DATA(0x08);
	LCD_WR_DATA(0x09);
	LCD_WR_DATA(0x09);
	LCD_WR_DATA(0x05);
	LCD_WR_DATA(0x2a);
	LCD_WR_DATA(0x33);
	LCD_WR_DATA(0x41);
	LCD_WR_DATA(0x07);
	LCD_WR_DATA(0x13);
	LCD_WR_DATA(0x13);
	LCD_WR_DATA(0x29);
	LCD_WR_DATA(0x2f);
	LCD_WR_REG(0xe1);
	LCD_WR_DATA(0x70);
	LCD_WR_DATA(0x03);
	LCD_WR_DATA(0x09);
	LCD_WR_DATA(0x0a);
	LCD_WR_DATA(0x09);
	LCD_WR_DATA(0x06);
	LCD_WR_DATA(0x2b);
	LCD_WR_DATA(0x34);
	LCD_WR_DATA(0x41);
	LCD_WR_DATA(0x07);
	LCD_WR_DATA(0x12);
	LCD_WR_DATA(0x14);
	LCD_WR_DATA(0x28);
	LCD_WR_DATA(0x2e);
		
	LCD_WR_REG(0x21);//启用反显	
	LCD_WR_REG(0x29);//Display On	
}

void LCD_BK_PWM_Init(void)
{
	RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
	
	GPIOA->OTYPER&=~GPIO_Pin_8;
	GPIOA->MODER&=~0x00030000;//A8
	GPIOA->MODER|= 0x00020000;//Alternate function mode
	GPIOA->AFR[1]&=~0x0000000F;
	GPIOA->AFR[1]|= 0x00000001;	
	
	TIM1->PSC = 0; //Set prescaler to 0
	TIM1->ARR = 100; //101
	TIM1->CCR1 = 0;//Turn off heating by default
	
	TIM1->CCMR1 |= TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1; //Select PWM mode 1 on OC2
	TIM1->CCER |= TIM_CCER_CC1E; // enable the output on OC2
	TIM1->BDTR |= TIM_BDTR_MOE; //Enable output	
	TIM1->CR1 |= TIM_CR1_CEN; //Enable counter
	TIM1->EGR |= TIM_EGR_UG; //Force update generation	
	
//	GPIOA->BSRRL=GPIO_Pin_8;
//	GPIOA->OTYPER&=~GPIO_Pin_8;		
//	GPIOA->MODER&=~0x00030000;//A8
//	GPIOA->MODER|= 0x00010000;//General purpose output mode
//	GPIOA->OSPEEDR|=0x00030000;
}

void LCD_Set_BK_PWM(uint8_t Duty_Cycle)
{
	TIM1->CCR1 = Duty_Cycle;
}

void LCD_Init(void)
{	
	LCD_HW_Init();
	LCD_BK_PWM_Init();
	LCD_CMD_Init();
	LCD_Clear(BLACK);
	LCD_Set_BK_PWM(40);
	POINT_COLOR=YELLOW;
	BACK_COLOR=BLACK;	
}

void LCD_Set_Cursor(uint8_t Xpos, uint8_t Ypos)
{
		LCD_CS_CLR;
	
	  LCD_RS_CLR;//写地址
		DATAOUT(0X2A);LCD_WR_CLR;LCD_WR_SET;
		LCD_RS_SET;
    DATAOUT(0x00);LCD_WR_CLR;LCD_WR_SET;
    DATAOUT(Xpos);LCD_WR_CLR;LCD_WR_SET;
    DATAOUT(0x00);LCD_WR_CLR;LCD_WR_SET;
    DATAOUT(0xEF);LCD_WR_CLR;LCD_WR_SET;
	
	  LCD_RS_CLR;//写地址
		DATAOUT(0X2B);LCD_WR_CLR;LCD_WR_SET;
		LCD_RS_SET;
    DATAOUT(0x00);LCD_WR_CLR;LCD_WR_SET;
    DATAOUT(Ypos);LCD_WR_CLR;LCD_WR_SET;
    DATAOUT(0x00);LCD_WR_CLR;LCD_WR_SET;
    DATAOUT(0xEF);LCD_WR_CLR;LCD_WR_SET;
	  LCD_RS_CLR;//写地址
	
		DATAOUT(0X2C);LCD_WR_CLR;LCD_WR_SET;
		LCD_RS_SET;  	
    LCD_CS_SET;	
}

void LCD_Set_Windows(uint8_t Xs, uint8_t Ys , uint8_t Xe, uint8_t Ye)
{
		LCD_CS_CLR;
	
	  LCD_RS_CLR;//写地址
		DATAOUT(0X2A);LCD_WR_CLR;LCD_WR_SET;
		LCD_RS_SET;
    DATAOUT(0x00);LCD_WR_CLR;LCD_WR_SET;
    DATAOUT(Xs);LCD_WR_CLR;LCD_WR_SET;
    DATAOUT(0x00);LCD_WR_CLR;LCD_WR_SET;
    DATAOUT(Xe-1);LCD_WR_CLR;LCD_WR_SET;
	
	  LCD_RS_CLR;//写地址
		DATAOUT(0X2B);LCD_WR_CLR;LCD_WR_SET;
		LCD_RS_SET;
    DATAOUT(0x00);LCD_WR_CLR;LCD_WR_SET;
    DATAOUT(Ys);LCD_WR_CLR;LCD_WR_SET;
    DATAOUT(0x00);LCD_WR_CLR;LCD_WR_SET;
    DATAOUT(Ye-1);LCD_WR_CLR;LCD_WR_SET;
	  LCD_RS_CLR;//写地址
	
		DATAOUT(0X2C);LCD_WR_CLR;LCD_WR_SET;
		LCD_RS_SET;  	
    LCD_CS_SET;	
}
//画点
//x,y:坐标
//POINT_COLOR:此点的颜色
void LCD_DrawPoint(u16 x,u16 y,u16 color)
{
    LCD_Set_Cursor(x,y);     //设置光标位置
		LCD_CS_CLR;
    LCD_WR_DATA(color>>8);
    LCD_WR_DATA(color&0xff);	
    LCD_CS_SET;	
}
//快速画点
//x,y:坐标
//color:颜色
void LCD_Fast_DrawPoint(u16 x,u16 y,u16 color)
{
		LCD_CS_CLR;
	
	  LCD_RS_CLR;//写地址
		DATAOUT(0X2A);LCD_WR_CLR;LCD_WR_SET;
		LCD_RS_SET;
    DATAOUT(0x00);LCD_WR_CLR;LCD_WR_SET;
    DATAOUT(x);LCD_WR_CLR;LCD_WR_SET;
    DATAOUT(0x00);LCD_WR_CLR;LCD_WR_SET;
    DATAOUT(0xEF);LCD_WR_CLR;LCD_WR_SET;
	
	  LCD_RS_CLR;//写地址
		DATAOUT(0X2B);LCD_WR_CLR;LCD_WR_SET;
		LCD_RS_SET;
    DATAOUT(0x00);LCD_WR_CLR;LCD_WR_SET;
    DATAOUT(y);LCD_WR_CLR;LCD_WR_SET;
    DATAOUT(0x00);LCD_WR_CLR;LCD_WR_SET;
    DATAOUT(0xEF);LCD_WR_CLR;LCD_WR_SET;
	  LCD_RS_CLR;//写地址
	
		DATAOUT(0X2C);LCD_WR_CLR;LCD_WR_SET;
		LCD_RS_SET;  	
    DATAOUT(color>>8);LCD_WR_CLR;LCD_WR_SET;
    DATAOUT(color&0xff);LCD_WR_CLR;LCD_WR_SET;
    LCD_CS_SET;	
}

void LCD_Fill(uint8_t sx,uint8_t sy,uint8_t ex,uint8_t ey,uint16_t color)
{
    uint32_t index; 
		uint32_t color_h;
		uint32_t color_l;
		uint32_t Nums=(ex-sx)*(ey-sy)/2;
	  LCD_Set_Windows(sx, sy , ex, ey);
	
		color_h=0x00FF0000|((color>>8)&0xff);
		color_l=0x00FF0000|(color&0xff);
	
		LCD_CS_CLR;
		LCD_RS_SET;
    for(index=0; index<Nums; index++)
    {
			*(__IO uint32_t*)(GPIOB_BASE+0x18)=color_h;LCD_WR_CLR;LCD_WR_SET;	
			*(__IO uint32_t*)(GPIOB_BASE+0x18)=color_l;LCD_WR_CLR;LCD_WR_SET;	
			*(__IO uint32_t*)(GPIOB_BASE+0x18)=color_h;LCD_WR_CLR;LCD_WR_SET;	
			*(__IO uint32_t*)(GPIOB_BASE+0x18)=color_l;LCD_WR_CLR;LCD_WR_SET;					
    }
		LCD_CS_SET;
}


//快速显示char 全填充
void LCD_ShowChar(uint8_t x,uint8_t y,uint8_t num,uint8_t size)
{
	uint8_t temp;
	uint32_t t,t1;

	LCD_WR_REG(0x36);    // Memory Access Control 
	if(size!=48)LCD_WR_DATA(0x20);//my mx mv ml rgb mh 0 0
		if(size==16)
			{
				num=num-' ';//???????
					LCD_Set_Windows(y,x,y+16,x+8); 
					LCD_CS_CLR;
					for(t=0;t<16;t++)
					{
						temp=asc2_1608[num][t];
						for(t1=0;t1<8;t1++)
						{
							if(temp&0x80)
							{
								LCD_Quick_WR_DATA(((uint8_t*)&POINT_COLOR)[1]);
								LCD_Quick_WR_DATA(((uint8_t*)&POINT_COLOR)[0]);
							}
							else 
							{
								LCD_Quick_WR_DATA(((uint8_t*)&BACK_COLOR)[1]);
								LCD_Quick_WR_DATA(((uint8_t*)&BACK_COLOR)[0]);
							}
							temp<<=1;
						}
					}			
					LCD_CS_SET;
			}
			else if(size==24)
			{
				num=num-' ';//???????
					LCD_Set_Windows(y,x,y+24,x+12); 
					LCD_CS_CLR;
					for(t=0;t<36;t++)
					{
						temp=asc2_2412[num][t];
						for(t1=0;t1<8;t1++)
						{
							if(temp&0x80)
							{
								LCD_Quick_WR_DATA(((uint8_t*)&POINT_COLOR)[1]);
								LCD_Quick_WR_DATA(((uint8_t*)&POINT_COLOR)[0]);
							}
							else 
							{
								LCD_Quick_WR_DATA(((uint8_t*)&BACK_COLOR)[1]);
								LCD_Quick_WR_DATA(((uint8_t*)&BACK_COLOR)[0]);
							}
							temp<<=1;
						}
					}			
					LCD_CS_SET;
			}
			else if(size==32)
			{
				num=num-' ';//???????
					LCD_Set_Windows(y,x,y+32,x+16); 
					LCD_CS_CLR;
					for(t=0;t<64;t++)
					{
						temp=asc2_3216[num][t];
						for(t1=0;t1<8;t1++)
						{
							if(temp&0x80)
							{
								LCD_Quick_WR_DATA(((uint8_t*)&POINT_COLOR)[1]);
								LCD_Quick_WR_DATA(((uint8_t*)&POINT_COLOR)[0]);
							}
							else 
							{
								LCD_Quick_WR_DATA(((uint8_t*)&BACK_COLOR)[1]);
								LCD_Quick_WR_DATA(((uint8_t*)&BACK_COLOR)[0]);
							}
							temp<<=1;
						}
					}		
					LCD_CS_SET;	
			}
			else if(size==48)
			{
				num=num-' ';
					LCD_Set_Windows(x,y,x+24,y+35); 
					LCD_CS_CLR;
					for(t=0;t<105;t++)
					{
						temp=asc2_4824[num][t];
						for(t1=0;t1<8;t1++)
						{
							if(temp&0x80)
							{
								LCD_Quick_WR_DATA(((uint8_t*)&POINT_COLOR)[1]);
								LCD_Quick_WR_DATA(((uint8_t*)&POINT_COLOR)[0]);
							}
							else 
							{
								LCD_Quick_WR_DATA(((uint8_t*)&BACK_COLOR)[1]);
								LCD_Quick_WR_DATA(((uint8_t*)&BACK_COLOR)[0]);
							}
							temp<<=1;
						}
					}			
					LCD_CS_SET;
			}
		LCD_WR_REG(0x36);    // Memory Access Control 
		LCD_WR_DATA(0x00);//my mx mv ml rgb mh 0 0
}

void LCD_ShowStr(uint8_t x,uint8_t y,uint8_t *str,uint8_t size)
{
	while(*str!=0)
	{
		LCD_ShowChar(x,y,*str++,size);
		x+=size/2;
	}
}

