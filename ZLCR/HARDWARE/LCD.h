#ifndef __LCD_H
#define __LCD_H	 
#include "sys.h"

extern uint16_t POINT_COLOR;
extern uint16_t BACK_COLOR;	

void LCD_Init(void);	//初始化		
void LCD_Clear(u16 color);


void LCD_Set_Cursor(uint8_t Xpos, uint8_t Ypos);
void LCD_Set_Windows(uint8_t Xs, uint8_t Ys , uint8_t Xe, uint8_t Ye);
void LCD_DrawPoint(u16 x,u16 y,u16 color);
void LCD_Fast_DrawPoint(u16 x,u16 y,u16 color);
void LCD_Fill(uint8_t sx,uint8_t sy,uint8_t ex,uint8_t ey,uint16_t color);

void LCD_ShowChar(uint8_t x,uint8_t y,uint8_t num,uint8_t size);
void LCD_ShowStr(uint8_t x,uint8_t y,uint8_t *str,uint8_t size);

//画笔颜色
#define WHITE        0xFFFF
#define BLACK        0x0000	  
#define BLUE         0x001F  
#define BRED         0XF81F
#define GRED 			   0XFFE0
#define GBLUE			   0X07FF
#define RED          0xF800
#define MAGENTA      0xF81F
#define GREEN        0x07E0
#define CYAN         0x7FFF
#define YELLOW       0xFFE0
#define BROWN 			 0XBC40 //棕色
#define BRRED 			 0XFC07 //棕红色
#define GRAY  			 0X8430 //灰色
//GUI颜色

#define DARKBLUE     0X01CF	//深蓝色
#define LIGHTBLUE    0X7D7C	//浅蓝色  
#define GRAYBLUE     0X5458 //灰蓝色
//以上三色为PANEL的颜色 
 
#define LIGHTGREEN   0X841F //浅绿色 
#define LGRAY 			 0XC618 //浅灰色(PANNEL),窗体背景色

#define LGRAYBLUE    0XA651 //浅灰蓝色(中间层颜色)
#define LBBLUE       0X2B12 //浅棕蓝色(选择条目的反色)








#endif

















