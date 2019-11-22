#include "usart.h"	  

//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
	/* Whatever you require here. If the only file you are using is */ 
	/* standard output using printf() for debugging, no file handling */ 
	/* is required. */ 
}; 
/* FILE is typedef’ d in stdio.h. */ 
FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定向fputc函数
//printf的输出，指向fputc，由fputc输出到串口
//这里使用串口1(USART1)输出printf信息
int fputc(int ch, FILE *f)
{      
	while((USART2->SR&0X40)==0);//等待上一次串口数据发送完成  
	USART2->DR = (u8) ch;      	//写DR,串口1将发送数据
	return ch;
}
#endif 
//end
//////////////////////////////////////////////////////////////////

#if EN_USART1_RX   //如果使能了接收

//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART_RX_STA=0;       //接收状态标记	  
  
void USART1_IRQHandler(void)
{
	u8 res;	
	if(USART1->SR&(1<<5))//接收到数据
	{	 
		res=USART1->DR; 
		if((USART_RX_STA&0x8000)==0)//接收未完成
		{
			if(USART_RX_STA&0x4000)//接收到了0x0d
			{
				if(res!=0x0a)USART_RX_STA=0;//接收错误,重新开始
				else USART_RX_STA|=0x8000;	//接收完成了 
			}else //还没收到0X0D
			{	
				if(res==0x0d)USART_RX_STA|=0x4000;
				else
				{
					USART_RX_BUF[USART_RX_STA&0X3FFF]=res;
					USART_RX_STA++;
					if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//接收数据错误,重新开始接收	  
				}		 
			}
		}  		 									     
	} 
} 
#endif										 
//初始化IO 串口2
//pclk2:PCLK2时钟频率(hz)
//bound:波特率 
void uart1_init(u32 pclk2,u32 bound)
{  	  
	RCC->APB2ENR|=1<<4;//使能串口1时钟  
	
	//波特率设置
 	USART1->BRR=pclk2/bound; 	//波特率设置		
	USART1->CR1=1<<3;  	//串口发送使能  16过采样 8位数据
#if EN_USART1_RX		  	//如果使能了接收
	//使能接收中断 
	USART1->CR1|=1<<2;  	//串口接收使能
	USART1->CR1|=1<<5;    	//接收缓冲区非空中断使能	    	
	MY_NVIC_Init(3,3,USART1_IRQn,2);//组2，最低优先级 
	GPIOA->MODER&=~0x003C0000;//PA9 10
	GPIOA->OSPEEDR|=0x003C0000;
	GPIOA->MODER|= 0x00280000;
	GPIOA->OTYPER&=~(GPIO_Pin_9|GPIO_Pin_10);
	GPIOA->AFR[1]&=~0x00000FF0;
	GPIOA->AFR[1]|= 0x00000770;
#else	
	GPIOA->MODER&=~0x000C0000;//PA9
	GPIOA->MODER|= 0x00080000;
	GPIOA->OSPEEDR|=0x00000200;// Fast speed
	GPIOA->OTYPER&=~GPIO_Pin_9;
	GPIOA->AFR[1]&=~0x000000F0;
	GPIOA->AFR[1]|= 0x00000070;
#endif
	
	USART1->CR1|=1<<13;  	//串口使能
}
