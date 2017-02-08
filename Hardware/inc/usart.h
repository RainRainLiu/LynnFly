#ifndef __UART1_H_
#define __UART1_H_

#include "gd32f10x.h"
#include "stdint.h"

#define USART_REC_LEN  			200  	//定义最大接收字节数 200
#define EN_USART1_RX 			1		//使能（1）/禁止（0）串口1接收

typedef struct
{
    uint16_t data_length;

    
}UartDMASend_Typedef;

TypeState UartPrintf(char *fmt, ...);
	  	

void uart_init(u32 bound);


#endif


