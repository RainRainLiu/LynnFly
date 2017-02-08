/**
  ******************************************************************************
  * @file    gd32_it.c
  * @author  MCU SD
  * @version V1.0.0
  * @date    26-Dec-2014
  * @brief   Main interrupt service routines
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "gd32_it.h"
//#include "systick.h"


void xPortPendSVHandler( void );
void xPortSysTickHandler( void );
void vPortSVCHandler( void );

/* Private variables ---------------------------------------------------------*/
//uint8_t Send_Buffer[4] = {0x00, 0x01, 0x00, 0x01};

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void  NMI_Handler (void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void  HardFault_Handler (void)
{
    /* Go to infinite loop when Hard Fault exception occurs */
    while (1)
    {
        //NVIC_SystemReset();
    }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void  SVC_Handler (void)
{
    vPortSVCHandler();
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void  PendSV_Handler (void)
{
    xPortPendSVHandler();
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void  SysTick_Handler (void)
{
    xPortSysTickHandler();
     //SysTick_Isr();
}

/**
  * @brief  This function handles USB FS Handler.
  * @param  None
  * @retval None
  */
void  USB_LP_CAN1_RX0_IRQHandler (void)
{
    //USB_Ifr();
    
}

#ifdef USB_DEVICE_LOW_PWR_MODE_SUPPORT

/**
  * @brief  This function handles USB WakeUp interrupt request.
  * @param  None
  * @retval None
  */
void  USBWakeUp_IRQHandler (void)
{
    EXTI_ClearITPendingBit(EXTI_Line18);
}

#endif

/**
  * @brief  This function handles EXTI0_1_IRQ Handler.
  * @param  None
  * @retval None
  */
void  EXTI0_IRQHandler (void)
{
}


/**
  * @brief  This function handles EXTI14_15_IRQ Handler.
  * @param  None
  * @retval None
  */
void  EXTI15_10_IRQHandler (void)
{
}

/************************ (C) COPYRIGHT 2014 GIGADEVICE *****END OF FILE****/
