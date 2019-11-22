#include "usart.h"	  

//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
	/* Whatever you require here. If the only file you are using is */ 
	/* standard output using printf() for debugging, no file handling */ 
	/* is required. */ 
}; 
/* FILE is typedef�� d in stdio.h. */ 
FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc����
//printf�������ָ��fputc����fputc���������
//����ʹ�ô���1(USART1)���printf��Ϣ
int fputc(int ch, FILE *f)
{      
	while((USART2->SR&0X40)==0);//�ȴ���һ�δ������ݷ������  
	USART2->DR = (u8) ch;      	//дDR,����1����������
	return ch;
}
#endif 
//end
//////////////////////////////////////////////////////////////////

#if EN_USART1_RX   //���ʹ���˽���

//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART_RX_STA=0;       //����״̬���	  
  
void USART1_IRQHandler(void)
{
	u8 res;	
	if(USART1->SR&(1<<5))//���յ�����
	{	 
		res=USART1->DR; 
		if((USART_RX_STA&0x8000)==0)//����δ���
		{
			if(USART_RX_STA&0x4000)//���յ���0x0d
			{
				if(res!=0x0a)USART_RX_STA=0;//���մ���,���¿�ʼ
				else USART_RX_STA|=0x8000;	//��������� 
			}else //��û�յ�0X0D
			{	
				if(res==0x0d)USART_RX_STA|=0x4000;
				else
				{
					USART_RX_BUF[USART_RX_STA&0X3FFF]=res;
					USART_RX_STA++;
					if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
				}		 
			}
		}  		 									     
	} 
} 
#endif										 
//��ʼ��IO ����2
//pclk2:PCLK2ʱ��Ƶ��(hz)
//bound:������ 
void uart1_init(u32 pclk2,u32 bound)
{  	  
	RCC->APB2ENR|=1<<4;//ʹ�ܴ���1ʱ��  
	
	//����������
 	USART1->BRR=pclk2/bound; 	//����������		
	USART1->CR1=1<<3;  	//���ڷ���ʹ��  16������ 8λ����
#if EN_USART1_RX		  	//���ʹ���˽���
	//ʹ�ܽ����ж� 
	USART1->CR1|=1<<2;  	//���ڽ���ʹ��
	USART1->CR1|=1<<5;    	//���ջ������ǿ��ж�ʹ��	    	
	MY_NVIC_Init(3,3,USART1_IRQn,2);//��2��������ȼ� 
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
	
	USART1->CR1|=1<<13;  	//����ʹ��
}
