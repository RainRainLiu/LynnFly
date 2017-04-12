/**
  ******************************************************************************
  * File Name          : USART.c
  * Description        : This file provides code for the configuration
  *                      of the USART instances.
  ******************************************************************************
  *
  * Copyright (c) 2017 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "usart.h"

#include "gpio.h"
#include "dma.h"

/* USER CODE BEGIN 0 */
#include <stdarg.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

typedef struct
{
    HAL_UART_ReceiveProcessCB rxCB; //接收回调
    UART_HandleTypeDef *huart;      //uart
    uint8_t rxData;                 //接收数据
    uint8_t *pTxData;               //发送数据
    uint8_t free;
    
}HAL_UART_HADNLE_T;

HAL_UART_HADNLE_T debugUart = 
{
    .huart = &huart1,
};

/* USER CODE END 0 */

UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_tx;

/* USART1 init function */

void MX_USART1_UART_Init(void)
{

  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();
  
    /**USART1 GPIO Configuration    
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* Peripheral DMA init*/
  
    hdma_usart1_tx.Instance = DMA1_Channel4;
    hdma_usart1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_usart1_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart1_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart1_tx.Init.Mode = DMA_NORMAL;
    hdma_usart1_tx.Init.Priority = DMA_PRIORITY_LOW;
    if (HAL_DMA_Init(&hdma_usart1_tx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmatx,hdma_usart1_tx);

  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();
  
    /**USART1 GPIO Configuration    
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX 
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);

    /* Peripheral DMA DeInit*/
    HAL_DMA_DeInit(uartHandle->hdmatx);
  }
  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
} 

/* USER CODE BEGIN 1 */

/******************************************************************
  * @函数说明：   串口printf输出
  * @输入参数：   标准printf输出
  * @输出参数：   无
  * @返回参数：   无             
  * @修改记录：   ----
******************************************************************/
ErrorStatus UartPrintf(char *fmt, ...)
{
    char *buff = NULL;//[64];
    int printed;
    
    buff = pvPortMalloc(256);
    
    if (buff == NULL)
    {
        return ERROR;
    }
    __va_list args;
    
    __va_start(args, fmt);
    
    printed = vsprintf(buff, fmt, args);        //转换数据
    
    __va_end(args);
    
    if (Hal_UART_SendString((uint8_t *)buff, printed) == ERROR)
    {
        vPortFree(buff);
        return ERROR;
    }
    
    vPortFree(buff);
    
    return SUCCESS;
}
/******************************************************************
  * @函数说明：   设置接收数据的回调函数
  * @输入参数：   HAL_UART_ReceiveProcessCB cb 回调函数
  * @输出参数：   无
  * @返回参数：   无             
  * @修改记录：   ----
******************************************************************/
ErrorStatus HAL_UART_SetReceiveCB(HAL_UART_ReceiveProcessCB cb)
{
    if (cb == NULL)
    {
        return ERROR;
    }
    
    debugUart.rxCB = cb;
    HAL_UART_Receive_IT(debugUart.huart, &debugUart.rxData, 1);
    return SUCCESS;
}

/******************************************************************
  * @函数说明：   串口接收中断处理，并掉用处理回调
  * @输入参数：   UART_HandleTypeDef *huart 串口
  * @输出参数：   
  * @返回参数：   无             
  * @修改记录：   ----
******************************************************************/
ErrorStatus Hal_UART_SendString(uint8_t *pData, uint32_t nLength)
{
    ErrorStatus status = SUCCESS;
    
    if (debugUart.free == 1 && debugUart.pTxData != NULL)
    {
        vPortFree(debugUart.pTxData);
    }
    debugUart.free = 0;
    
    debugUart.pTxData = pvPortMalloc(nLength);
    
    if (debugUart.pTxData != NULL)
    {
        
        memcpy(debugUart.pTxData, pData, nLength);
        if (HAL_UART_Transmit_DMA(debugUart.huart, debugUart.pTxData, nLength) == HAL_OK)
        {
           status = SUCCESS; 
        }
        else
        {
            vPortFree(debugUart.pTxData);
            debugUart.pTxData = NULL;
            status = ERROR;
        }
    }
    else
    {
        status = ERROR;
    }
    return status;
}

/******************************************************************
  * @函数说明：   串口接收中断处理，并掉用处理回调
  * @输入参数：   UART_HandleTypeDef *huart 串口
  * @输出参数：   
  * @返回参数：   无             
  * @修改记录：   ----
******************************************************************/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (debugUart.rxCB != NULL)
    {
        debugUart.rxCB(debugUart.rxData);
    }
    HAL_UART_Receive_IT(huart, &debugUart.rxData, 1);
    
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    debugUart.free = 1;
}


/* USER CODE END 1 */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
