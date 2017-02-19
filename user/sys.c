#include "sys.h"
#include "usart.h"	
#include "IIC.h"

//********************************************************************************  
void NVIC_Configuration(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
}

void SysTick_Init(void)
{    
	uint32_t sysClock = SystemCoreClock;
	
	SysTick->CTRL &= ~(1 << 3);						// SysTick_CLKSource_HCLK_Div8;	// ��Ƶ
	
	SysTick->LOAD = (sysClock / 9 ) / 1000;			// 1ms
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;		// ����SYSTICK�ж�
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;		// ����SYSTICK
}

void SysInit(void)
{
    NVIC_Configuration();
    
    SysTick_Init();
    
    Uart_init(115200);
    
    IIC_Init();
    
    
    OS_Printf("System Init Sucess\r\n");
}

