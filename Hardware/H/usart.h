#ifndef __USART_H
#define __USART_H

#include "stm32f10x.h"

#define     UART_DMA_CACHE_SIZE     256




#define	DBG_PRINTF(format, args...)	UartPrintf(&dmaHandle, format, ##args)


typedef struct
{
    uint16_t dataLength;
    
    DMA_Channel_TypeDef* DMAy_Channelx;
    
    
}UART_DMA_SEND_HANDLE_T;

extern UART_DMA_SEND_HANDLE_T dmaHandle;

void Uart_init(u32 bound);
void Uart_SendByte(USART_TypeDef* USARTx, uint8_t data);
void Uart_SendString(USART_TypeDef* USARTx, uint8_t *pBuff, uint32_t nLength);

ErrorStatus UartDMA_SendString(UART_DMA_SEND_HANDLE_T *sendHandle, uint8_t *pBuff, uint32_t nLength);
ErrorStatus UartPrintf(UART_DMA_SEND_HANDLE_T *sendHandle, char *fmt, ...);

#endif


