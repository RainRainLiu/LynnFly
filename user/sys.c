
#include "sys.h"

#include "usart.h"	
#include "delay.h"
#include "IIC.h"
#include "DMP.h"
#include "EXTI.h"
#include "NRF24L01.h"
#include "tim.h"
#include "moto.h"
//********************************************************************************  
void NVIC_Configuration(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
}

void SysTick_Init(void)
{
	uint32_t sysClock = SystemCoreClock;
	
	SysTick->CTRL &= ~(1 << 3);						// SysTick_CLKSource_HCLK_Div8;	// ��Ƶ
	
	SysTick->LOAD = (sysClock / 8 ) / 1000;			// 1ms
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;		// ����SYSTICK�ж�
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;		// ����SYSTICK
}

void SysInit(void)
{
    NVIC_Configuration();
    
    SysTick_Init();
    
    
}

