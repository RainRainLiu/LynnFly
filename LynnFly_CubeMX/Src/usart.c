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


HAL_UART_HADNLE_T *handleList[HAL_UART_NUMBER] = {NULL};


/* USER CODE END 0 */

UART_HandleTypeDef huart1;

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

    /* Peripheral interrupt init */
    HAL_NVIC_SetPriority(USART1_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
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

    /* Peripheral interrupt Deinit*/
    HAL_NVIC_DisableIRQ(USART1_IRQn);

  }
  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
} 

/* USER CODE BEGIN 1 */


HAL_UART_HADNLE_T* Hal_UART_GetHandle(UART_HandleTypeDef* huart)
{
    uint32_t i;
    
    for (i = 0; i < HAL_UART_NUMBER; i++)
    {
        if (handleList[i] != NULL)
        {
            if (handleList[i]->huart == huart)  //找到，返回
            {
                return handleList[i];
            }
        }
    }
    
    for (i = 0; i < HAL_UART_NUMBER; i++)   //为找到，创建新的
    {
        if (handleList[i] == NULL)
        {
            handleList[i] = osMalloc(sizeof(HAL_UART_HADNLE_T));
            if (handleList[i] != NULL)
            {
                handleList[i]->huart = huart;
            }
            return handleList[i];
        }
    }
    
    return NULL;//错误
}



/******************************************************************
  * @函数说明：   串口接收中断处理，并掉用处理回调
  * @输入参数：   UART_HandleTypeDef *huart 串口
  * @输出参数：   
  * @返回参数：   无             
  * @修改记录：   ----
******************************************************************/
ErrorStatus Hal_UART_SendString(UART_HandleTypeDef* huart, uint8_t *pData, uint32_t nLength)
{
    ErrorStatus status = SUCCESS;
    
    HAL_UART_HADNLE_T *uartHandle = Hal_UART_GetHandle(huart);
    if (uartHandle == NULL) //错误
    {
        return ERROR;
    }
    
    while(uartHandle->free == 0 && uartHandle->pTxData != NULL)
    {
        osDelay(1);
    }
    
    if (uartHandle->free == 1 && uartHandle->pTxData != NULL)
    {
        vPortFree(uartHandle->pTxData);
    }
    uartHandle->free = 0;
    
    uartHandle->pTxData = pvPortMalloc(nLength);
    
    if (uartHandle->pTxData != NULL)
    {
        memcpy(uartHandle->pTxData, pData, nLength);
        if (HAL_UART_Transmit_IT(uartHandle->huart, uartHandle->pTxData, nLength) == HAL_OK)
        {
           status = SUCCESS; 
        }
        else
        {
            vPortFree(uartHandle->pTxData);
            uartHandle->pTxData = NULL;
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
  * @函数说明：   串口printf输出
  * @输入参数：   标准printf输出
  * @输出参数：   无
  * @返回参数：   无             
  * @修改记录：   ----
******************************************************************/
ErrorStatus UartPrintf(UART_HandleTypeDef* huart, char *fmt, ...)
{
    char *buff = NULL;
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
    
    if (Hal_UART_SendString(huart, (uint8_t *)buff, printed) == ERROR)
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
ErrorStatus HAL_UART_SetReceiveCB(UART_HandleTypeDef* huart, HAL_UART_ReceiveProcessCB cb)
{
    if (cb == NULL)
    {
        return ERROR;
    }
    
    HAL_UART_HADNLE_T *uartHandle = Hal_UART_GetHandle(huart);
    if (uartHandle == NULL) //错误
    {
        return ERROR;
    }
    
    uartHandle->rxCB = cb;
    HAL_UART_Receive_IT(uartHandle->huart, &uartHandle->rxData, 1);
    return SUCCESS;
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
    HAL_UART_HADNLE_T *uartHandle = Hal_UART_GetHandle(huart);
    
    if (uartHandle != NULL) //错误
    {
        if (uartHandle->rxCB != NULL)
        {
            uartHandle->rxCB(uartHandle->rxData);
        }
        HAL_UART_Receive_IT(huart, &uartHandle->rxData, 1);
    }
    
    
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    HAL_UART_HADNLE_T *uartHandle = Hal_UART_GetHandle(huart);
    
    if (uartHandle != NULL) //错误
    {
        uartHandle->free = 1;
    }
}


/* USER CODE END 1 */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
