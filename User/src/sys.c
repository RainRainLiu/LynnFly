#include "sys.h"
#include "gd32f10x.h"
#include "usart.h"	
#include "gd32f10x_rcc.h"
#include "gd32f10x_misc.h"

void SysTick_Init(void)
{
	uint32_t sysClock = SystemCoreClock;
	
	SysTick->CTRL &= ~(1 << 3);						// SysTick_CLKSource_HCLK_Div8;	// 分频
	
	SysTick->LOAD = (sysClock / 8 ) / 1000;			// 1ms
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;		// 开启SYSTICK中断
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;		// 开启SYSTICK
}
/*******************************************************
  * @函数名称：   System_Clock_Init
  * @函数说明：   系统时钟初始化，48MHz
  * @输入参数：   
  * @返回参数：   
  * @修改记录：   
******************************************************************/
void System_Clock_Init(void) 
{ 
	RCC_DeInit();                //将外设 RCC寄存器重设为缺省值    
    RCC_HSI_Enable(ENABLE);  
        
	while(RCC_GetBitState(RCC_FLAG_HSISTB) == RESET) {}      //等待HSI使能成功        
	if (1)   
	{ 

        RCC_AHBConfig(RCC_SYSCLK_DIV1);
        RCC_APB1Config(RCC_APB1AHB_DIV1);
        RCC_APB2Config(RCC_APB2AHB_DIV2);
     
		//设置 PLL 时钟源及倍频系数 
		RCC_PLLConfig(RCC_PLLSOURCE_HSI_DIV2, RCC_PLLMUL_12);                    
		//使能或者失能 PLL,这个参数可以取：ENABLE或者DISABLE    
		//RCC_PLLCmd(ENABLE);            //如果PLL被用于系统时钟,那么它不能被失能 
        RCC_PLL_Enable(ENABLE);
		//等待指定的 RCC 标志位设置成功 等待PLL初始化成功    
		while(RCC_GetBitState(RCC_FLAG_PLLSTB) == RESET){  }   
		//设置系统时钟（SYSCLK） 设置PLL为系统时钟源    
		
       // RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);   //等待PLL成功用作于系统时钟的时钟源     //  0x00：HSI 作为系统时钟      //  0x04：HSE作为系统时钟      //  0x08：PLL作为系统时钟   
        RCC_CK_SYSConfig(RCC_SYSCLKSOURCE_PLLCLK);
        
        while(RCC_GetCK_SYSSource() != 0x08){  } 
        
            
       

        RCC_APB1PeriphClock_Enable(RCC_APB1PERIPH_BKP | RCC_APB1PERIPH_PWR, ENABLE);
        RCC_APB2PeriphClock_Enable(RCC_APB2PERIPH_GPIOA | RCC_APB2PERIPH_GPIOB | RCC_APB2PERIPH_AF,ENABLE);
        
        NVIC_PRIGroup_Enable(NVIC_PRIGROUP_2);
	} 
}





