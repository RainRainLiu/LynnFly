#include "sys.h"
#include "gd32f10x.h"
#include "usart.h"	
#include "gd32f10x_rcc.h"
#include "gd32f10x_misc.h"

void SysTick_Init(void)
{
	uint32_t sysClock = SystemCoreClock;
	
	SysTick->CTRL &= ~(1 << 3);						// SysTick_CLKSource_HCLK_Div8;	// ��Ƶ
	
	SysTick->LOAD = (sysClock / 8 ) / 1000;			// 1ms
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;		// ����SYSTICK�ж�
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;		// ����SYSTICK
}
/*******************************************************
  * @�������ƣ�   System_Clock_Init
  * @����˵����   ϵͳʱ�ӳ�ʼ����48MHz
  * @���������   
  * @���ز�����   
  * @�޸ļ�¼��   
******************************************************************/
void System_Clock_Init(void) 
{ 
	RCC_DeInit();                //������ RCC�Ĵ�������Ϊȱʡֵ    
    RCC_HSI_Enable(ENABLE);  
        
	while(RCC_GetBitState(RCC_FLAG_HSISTB) == RESET) {}      //�ȴ�HSIʹ�ܳɹ�        
	if (1)   
	{ 

        RCC_AHBConfig(RCC_SYSCLK_DIV1);
        RCC_APB1Config(RCC_APB1AHB_DIV1);
        RCC_APB2Config(RCC_APB2AHB_DIV2);
     
		//���� PLL ʱ��Դ����Ƶϵ�� 
		RCC_PLLConfig(RCC_PLLSOURCE_HSI_DIV2, RCC_PLLMUL_12);                    
		//ʹ�ܻ���ʧ�� PLL,�����������ȡ��ENABLE����DISABLE    
		//RCC_PLLCmd(ENABLE);            //���PLL������ϵͳʱ��,��ô�����ܱ�ʧ�� 
        RCC_PLL_Enable(ENABLE);
		//�ȴ�ָ���� RCC ��־λ���óɹ� �ȴ�PLL��ʼ���ɹ�    
		while(RCC_GetBitState(RCC_FLAG_PLLSTB) == RESET){  }   
		//����ϵͳʱ�ӣ�SYSCLK�� ����PLLΪϵͳʱ��Դ    
		
       // RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);   //�ȴ�PLL�ɹ�������ϵͳʱ�ӵ�ʱ��Դ     //  0x00��HSI ��Ϊϵͳʱ��      //  0x04��HSE��Ϊϵͳʱ��      //  0x08��PLL��Ϊϵͳʱ��   
        RCC_CK_SYSConfig(RCC_SYSCLKSOURCE_PLLCLK);
        
        while(RCC_GetCK_SYSSource() != 0x08){  } 
        
            
       

        RCC_APB1PeriphClock_Enable(RCC_APB1PERIPH_BKP | RCC_APB1PERIPH_PWR, ENABLE);
        RCC_APB2PeriphClock_Enable(RCC_APB2PERIPH_GPIOA | RCC_APB2PERIPH_GPIOB | RCC_APB2PERIPH_AF,ENABLE);
        
        NVIC_PRIGroup_Enable(NVIC_PRIGROUP_2);
	} 
}





