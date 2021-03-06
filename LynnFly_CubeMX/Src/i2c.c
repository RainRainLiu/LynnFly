/**
  ******************************************************************************
  * File Name          : I2C.c
  * Description        : This file provides code for the configuration
  *                      of the I2C instances.
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
#include "i2c.h"

#include "gpio.h"

/* USER CODE BEGIN 0 */
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

/* USER CODE END 0 */

I2C_HandleTypeDef hi2c1;

/* I2C1 init function */
void MX_I2C1_Init(void)
{

  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 400000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

}

void HAL_I2C_MspInit(I2C_HandleTypeDef* i2cHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  if(i2cHandle->Instance==I2C1)
  {
  /* USER CODE BEGIN I2C1_MspInit 0 */

  /* USER CODE END I2C1_MspInit 0 */
  
    /**I2C1 GPIO Configuration    
    PB6     ------> I2C1_SCL
    PB7     ------> I2C1_SDA 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* Peripheral clock enable */
    __HAL_RCC_I2C1_CLK_ENABLE();

    /* Peripheral interrupt init */
    HAL_NVIC_SetPriority(I2C1_EV_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(I2C1_EV_IRQn);
    HAL_NVIC_SetPriority(I2C1_ER_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(I2C1_ER_IRQn);
  /* USER CODE BEGIN I2C1_MspInit 1 */

  /* USER CODE END I2C1_MspInit 1 */
  }
}

void HAL_I2C_MspDeInit(I2C_HandleTypeDef* i2cHandle)
{

  if(i2cHandle->Instance==I2C1)
  {
  /* USER CODE BEGIN I2C1_MspDeInit 0 */

  /* USER CODE END I2C1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_I2C1_CLK_DISABLE();
  
    /**I2C1 GPIO Configuration    
    PB6     ------> I2C1_SCL
    PB7     ------> I2C1_SDA 
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6|GPIO_PIN_7);

    /* Peripheral interrupt Deinit*/
    HAL_NVIC_DisableIRQ(I2C1_EV_IRQn);

    HAL_NVIC_DisableIRQ(I2C1_ER_IRQn);

  }
  /* USER CODE BEGIN I2C1_MspDeInit 1 */

  /* USER CODE END I2C1_MspDeInit 1 */
} 

/* USER CODE BEGIN 1 */
FlagStatus completeFlag;
QueueHandle_t completeQueue = NULL;

/******************************************************************
  * @函数说明：   等待IIC操作完成
  * @输入参数：   uint32_t nTimeOut 等待时间
  * @输出参数：   
  * @返回参数：   无             
  * @修改记录：   ----
******************************************************************/
static ErrorStatus Hal_I2C_WaitComplete(uint32_t nTimeOut)
{
    if (completeQueue == NULL)
    {
        completeQueue = xQueueCreate(1,1);
    }
    uint8_t data;
    
    if (xQueueReceive(completeQueue, &data, nTimeOut) == pdTRUE)
    {
        return SUCCESS;
    }
    else
    {
        return ERROR;
    }
}
/******************************************************************
  * @函数说明：   IIC操作完成
  * @输入参数：   无
  * @输出参数：   
  * @返回参数：   无             
  * @修改记录：   ----
******************************************************************/
static void Hal_I2C_Complete(void)
{
    uint8_t data;
    BaseType_t xHigherPriorityTaskWoken;
    xQueueSendFromISR(completeQueue, &data, &xHigherPriorityTaskWoken);
}

/******************************************************************
  * @函数说明：   IIC读寄存器，采用中断方式，等待完成才返回
                 必须在任务中调用
  * @输入参数：   I2C_HandleTypeDef *hi2c,   i2c句柄
                  uint16_t DevAddress,      设备地址
                  uint16_t MemAddress,      寄存器地址
                  uint16_t MemAddSize,      寄存器地址大小
                  uint8_t *pData,           数据
                  uint16_t Size             数据长度
  * @输出参数：   
  * @返回参数：   无             
  * @修改记录：   ----
******************************************************************/
ErrorStatus Hal_I2C_ReadRegister(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint8_t *pData, uint16_t Size)
{
    if (HAL_I2C_Mem_Read_IT(hi2c, DevAddress, MemAddress,1, pData, Size) != HAL_OK)
    {
        return ERROR;
    }
    
    return Hal_I2C_WaitComplete(1000);
}
/******************************************************************
  * @函数说明：   IIC写寄存器，采用中断方式，等待完成才返回
                 必须在任务中调用
  * @输入参数：   I2C_HandleTypeDef *hi2c,   i2c句柄
                  uint16_t DevAddress,      设备地址
                  uint16_t MemAddress,      寄存器地址
                  uint16_t MemAddSize,      寄存器地址大小
                  uint8_t *pData,           数据
                  uint16_t Size             数据长度
  * @输出参数：   
  * @返回参数：   无             
  * @修改记录：   ----
******************************************************************/
ErrorStatus Hal_I2C_WriteRegister(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint8_t *pData, uint16_t Size)
{
    if (HAL_I2C_Mem_Write_IT(hi2c, DevAddress, MemAddress,1, pData, Size) != HAL_OK)
    {
        return ERROR;
    }
    
    return Hal_I2C_WaitComplete(1000);
}

/******************************************************************
  * @函数说明：   发送完毕的回调函数
  * @输入参数：   
  * @输出参数：   
  * @返回参数：   无             
  * @修改记录：   ----
******************************************************************/
void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    Hal_I2C_Complete();
}
/******************************************************************
  * @函数说明：   接收完毕的回调函数
  * @输入参数：   
  * @输出参数：   
  * @返回参数：   无             
  * @修改记录：   ----
******************************************************************/
void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    Hal_I2C_Complete();
}
/* USER CODE END 1 */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
