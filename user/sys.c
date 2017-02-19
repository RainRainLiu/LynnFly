#include "sys.h"
#include "usart.h"	
#include "IIC.h"

//********************************************************************************  
void NVIC_Configuration(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//设置NVIC中断分组2:2位抢占优先级，2位响应优先级
}

void SysTick_Init(void)
{    
	uint32_t sysClock = SystemCoreClock;
	
	SysTick->CTRL &= ~(1 << 3);						// SysTick_CLKSource_HCLK_Div8;	// 分频
	
	SysTick->LOAD = (sysClock / 9 ) / 1000;			// 1ms
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;		// 开启SYSTICK中断
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;		// 开启SYSTICK
}

void SysInit(void)
{
    NVIC_Configuration();
    
    SysTick_Init();
    
    Uart_init(115200);
    
    IIC_Init();
    
    
    OS_Printf("System Init Sucess\r\n");
}

