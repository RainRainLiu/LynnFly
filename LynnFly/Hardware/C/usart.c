#include "sys.h"
#include "usart.h"
#include <stdarg.h>
#include <string.h>
#include "stdio.h"	

char UartSendBuff[UART_DMA_CACHE_SIZE];

UART_DMA_SEND_HANDLE_T dmaHandle = 
{
    .DMAy_Channelx = DMA1_Channel4,
};



/******************************************************************
  * @函数名称：   uart_dma_init
  * @函数说明：   串口DMA初始化，对应通道DMA4，USART1
  * @输入参数：   无
  * @输出参数：   无       
  * @返回参数：   无             
  * @修改记录：   
******************************************************************/
void uart_dma_init(void)
{
    
    DMA_InitTypeDef DMA_InitStructure;
 
    //启动DMA时钟  
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1 | RCC_AHBPeriph_DMA2, ENABLE);  

      
    DMA_DeInit(DMA1_Channel4);                                              //DMA1通道4配置
                                                                            
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART1->DR);           //外设地址  
                                                                             
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)UartSendBuff;           //内存地址  
                                                                             
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;                       //dma传输方向单向  
                                                                             
    DMA_InitStructure.DMA_BufferSize = 0;                                  //设置DMA在传输时缓冲区的长度  
                                                                             
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;         //设置DMA的外设递增模式，一个外设  
                                                                             
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                  //设置DMA的内存递增模式  
                                                                             
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  //外设数据字长  
                                                                              
    DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_Byte;      //内存数据字长  
                                                                             
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                            //设置DMA的传输模式  
                                                                             
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;                    //设置DMA的优先级别  
                                                                             
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                             //设置DMA的2个memory中的变量互相访问
    
    
    DMA_Init(DMA1_Channel4,&DMA_InitStructure);  

    
    USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE);    //采用DMA方式发送  

}
/******************************************************************
  * @函数名称：   Uart_init
  * @函数说明：   u32 bound  波特率
  * @输入参数：   无
  * @输出参数：   无       
  * @返回参数：                
  * @修改记录：   
******************************************************************/
void Uart_init(u32 bound)
{
    GPIO_InitTypeDef    GPIO_InitStructure;
    USART_InitTypeDef   USART_InitStructure;
    NVIC_InitTypeDef    NVIC_InitStructure;
    
    //RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟
 	USART_DeInit(USART1);  //复位串口1
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd    = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
    
    USART_DeInit( USART1 );
    USART_InitStructure.USART_BaudRate              = bound;
    USART_InitStructure.USART_WordLength            = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits              = USART_StopBits_1;
    USART_InitStructure.USART_Parity                = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl   = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode                  = USART_Mode_Rx | USART_Mode_Tx;


    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    
    USART_Init(USART1, &USART_InitStructure);                    // 初始化串口
    
    USART_Cmd(USART1, ENABLE); 
    
    uart_dma_init();
    
}



/******************************************************************
  * @函数名称：   Uart_SendByte
  * @函数说明：   串口字符串输出
  * @输入参数：   USART_TypeDef* USARTx,     串口号 
                  uint8_t data,            目标数据
  * @输出参数：   无       
  * @返回参数：   无             
  * @修改记录：   
******************************************************************/
void Uart_SendByte(USART_TypeDef* USARTx, uint8_t data)
{
    USART_ClearFlag(USARTx,USART_FLAG_TC);
    
    USART_SendData(USARTx, data);
    
    while(USART_GetFlagStatus(USARTx,USART_FLAG_TC) == RESET);
}

/******************************************************************
  * @函数名称：   Uart_SendString
  * @函数说明：   串口字符串输出
  * @输入参数：   USART_TypeDef* USARTx,     串口号 
                 uint8_t *pBuff,            目标数据
                 uint32_t nLength           数据长度
  * @输出参数：   无       
  * @返回参数：   无             
  * @修改记录：   
******************************************************************/
void Uart_SendString(USART_TypeDef* USARTx, uint8_t *pBuff, uint32_t nLength)
{
    uint32_t i;
    
    for(i = 0; i < nLength; i++)
    {
        Uart_SendByte(USARTx, *(uint8_t*)(pBuff + i));
    }
}
/******************************************************************
  * @函数名称：   UartDMA_SendString
  * @函数说明：   DMA输出串口信息，缓存满则不接收新的数据
  * @输入参数：   UART_DMA_SEND_HANDLE_T *sendHandle, 使用的DMA句柄
                 uint8_t *pBuff,    目标数据
                 uint32_t nLength   数据长度
  * @输出参数：   当前要发送的总数据长度       
  * @返回参数：   发送是否成功（缓存满则返回失败）             
  * @修改记录：   
******************************************************************/
ErrorStatus UartDMA_SendString(UART_DMA_SEND_HANDLE_T *sendHandle, uint8_t *pBuff, uint32_t nLength)
{
    uint32_t remaining_length;
    
    remaining_length = DMA_GetCurrDataCounter(sendHandle->DMAy_Channelx);   //读取DMA目前指针位置，计算剩余多少数据没输出
    
    
    if ((remaining_length + nLength) > UART_DMA_CACHE_SIZE)        //缓存满了，存不下，返回错误
    {
        return ERROR;
    }
    DMA_Cmd(sendHandle->DMAy_Channelx, DISABLE);             //把DMA停下了

    if (remaining_length > 0 && sendHandle->dataLength > remaining_length)  //缓存内还有未发送的数据
    {
        memcpy(UartSendBuff, &UartSendBuff[sendHandle->dataLength - remaining_length], remaining_length);      //把本次需要打印的信息粘贴到剩余信息的末尾，并且把缓存内已输出的信息去除
    }
    
    memcpy(&UartSendBuff[remaining_length], pBuff, nLength);
    
    sendHandle->dataLength = nLength + remaining_length;           //还需要输出的信息长度
    
    DMA_SetCurrDataCounter(sendHandle->DMAy_Channelx, nLength + remaining_length);      //设置DMA输出长度 
    
    DMA_Cmd(sendHandle->DMAy_Channelx, ENABLE); 
    
    return  SUCCESS;
}


ErrorStatus UartPrintf(UART_DMA_SEND_HANDLE_T *sendHandle, char *fmt, ...)
{
    char buff[128];
    int printed;
//    uint32_t remaining_length;
    
    __va_list args;
    
    __va_start(args, fmt);
    
    printed = vsprintf(buff, fmt, args);        //转换数据
    
    __va_end(args);
    
    
    return UartDMA_SendString(sendHandle, (uint8_t *)buff, printed);
}






