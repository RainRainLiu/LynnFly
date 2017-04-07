#include "stm32f10x.h"
#include "EXTI.h"
#include "stm32f10x_exti.h"
#include "DMP.h"
#include "usart.h"	  
// GPIO_EXTI NVIC≈‰÷√
void Exti_NVIC_Configuration(void) 
{ 
	NVIC_InitTypeDef NVIC_InitStructure; 
	
	//≈‰÷√PB3£¨4£¨5∫Õ“‘º∞PB1ø⁄£®≥‰µÁªΩ–—£©
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure);    
} 

// GPIO_EXTI ≈‰÷√
void Exti_Configuration(void) 
{ 
	EXTI_InitTypeDef EXTI_InitStructure; 
	GPIO_InitTypeDef  GPIO_InitStructure;
    
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_5; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;			
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
}



void EXTI9_5_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line5)!= RESET) 
	{ 
		EXTI_ClearITPendingBit(EXTI_Line5); 
	} 

}






