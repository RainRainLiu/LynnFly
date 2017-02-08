/******************************************************************
                DMA 串口打印，非阻塞式
****************************************************************/

#include "usart.h"	  
#include <stdarg.h>
#include <string.h>

#ifdef USE_DBG_OUT

char UartSendBuff[256];

UartDMASend_Typedef Uart_DMA_TX;


void uart_dma_init(void);
 


void uart_init(u32 bound)
{
    GPIO_InitPara GPIO_InitStructure;
    USART_InitPara USART_InitStructure;
    NVIC_InitPara   NVIC_InitStructure;
    
    /* Enable USART1 APB clock */
    RCC_APB2PeriphClock_Enable( RCC_APB2PERIPH_USART1 , ENABLE );
    
    GPIO_PinRemapConfig(GPIO_REMAP_USART1,ENABLE);
    
    
    GPIO_InitStructure.GPIO_Pin     = GPIO_PIN_11 | GPIO_PIN_12;
    GPIO_InitStructure.GPIO_Mode    = GPIO_MODE_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed   = GPIO_SPEED_50MHZ;
    GPIO_Init(GPIOA, &GPIO_InitStructure); 
    
    
    GPIO_InitStructure.GPIO_Pin     = GPIO_PIN_6 ;
    GPIO_InitStructure.GPIO_Mode    = GPIO_MODE_AF_PP;
    GPIO_InitStructure.GPIO_Speed   = GPIO_SPEED_50MHZ;
    GPIO_Init( GPIOB , &GPIO_InitStructure); 
    GPIO_InitStructure.GPIO_Pin     = GPIO_PIN_7;
    GPIO_InitStructure.GPIO_Mode    = GPIO_MODE_IN_FLOATING;;
    GPIO_Init( GPIOB , &GPIO_InitStructure);
    
	

	//Usart1 NVIC 配置    
    NVIC_InitStructure.NVIC_IRQ = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQPreemptPriority = 5;
    NVIC_InitStructure.NVIC_IRQSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQEnable    = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	                           // 根据指定的参数初始化VIC寄存器
    
    
    
    USART_DeInit( USART1 );
    USART_InitStructure.USART_BRR           = bound;
    USART_InitStructure.USART_WL            = USART_WL_8B;
    USART_InitStructure.USART_STBits            = USART_STBITS_1;
    USART_InitStructure.USART_Parity                = USART_PARITY_RESET;
    USART_InitStructure.USART_HardwareFlowControl = USART_HARDWAREFLOWCONTROL_NONE;
    USART_InitStructure.USART_RxorTx                = USART_RXORTX_RX | USART_RXORTX_TX;
    USART_Init(USART1, &USART_InitStructure);
    
	USART_Init(USART1, &USART_InitStructure);                    // 初始化串口
    
    USART_Enable(USART1, ENABLE); 
    uart_dma_init();
}


void uart_dma_init(void)
{
    NVIC_InitPara   NVIC_InitStructure;
    
    DMA_InitPara    DMA_InitStructure;
    
 
    //启动DMA时钟  
    //RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_DMA1, ENABLE);  
    
    RCC_AHBPeriphClock_Enable(RCC_AHBPERIPH_DMA1, ENABLE);
    
    
    NVIC_InitStructure.NVIC_IRQ = DMA1_Channel4_IRQn;           //DMA发送中断设置 
    NVIC_InitStructure.NVIC_IRQPreemptPriority = 3;
    NVIC_InitStructure.NVIC_IRQSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQEnable    = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	                           // 根据指定的参数初始化VIC寄存器
    
    
                                                                 
  
      
    DMA_DeInit(DMA1_CHANNEL4);                                              //DMA1通道4配置
                                                                            
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART1->DR);           //外设地址  
                                                                             
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)UartSendBuff;           //内存地址  
                                                                             
    DMA_InitStructure.DMA_DIR = DMA_DIR_PERIPHERALDST;//DMA_DIR_PeripheralDST;                       //dma传输方向单向  
                                                                             
    DMA_InitStructure.DMA_BufferSize = 0;                                  //设置DMA在传输时缓冲区的长度  
                                                                             
    DMA_InitStructure.DMA_PeripheralInc = DMA_PERIPHERALINC_DISABLE; //DMA_PeripheralInc_Disable;         //设置DMA的外设递增模式，一个外设  
                                                                             
    DMA_InitStructure.DMA_MemoryInc = DMA_MEMORYINC_ENABLE;//DMA_MemoryInc_Enable;                  //设置DMA的内存递增模式  
                                                                             
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PERIPHERALDATASIZE_BYTE;//DMA_PeripheralDataSize_Byte;  //外设数据字长  
                                                                              
    DMA_InitStructure.DMA_MemoryDataSize = DMA_PERIPHERALDATASIZE_BYTE;//DMA_PeripheralDataSize_Byte;      //内存数据字长  
                                                                             
    DMA_InitStructure.DMA_Mode = DMA_MODE_NORMAL;//DMA_Mode_Normal;                            //设置DMA的传输模式  
                                                                             
    DMA_InitStructure.DMA_Priority = DMA_PRIORITY_MEDIUM;//DMA_Priority_Medium;                    //设置DMA的优先级别  
                                                                             
    //DMA_InitStructure.DMA_M2M = DMA_M//DMA_M2M_Disable;                             //设置DMA的2个memory中的变量互相访问
    DMA_InitStructure.DMA_MTOM = DMA_MEMTOMEM_DISABLE;
    
    DMA_Init(DMA1_CHANNEL4,&DMA_InitStructure);  
 
    DMA_INTConfig(DMA1_CHANNEL4, DMA_INT_TC, ENABLE);
    //采用DMA方式发送
    USART_DMA_Enable(USART1, USART_DMAREQ_TX, ENABLE);

      


}

void USART1_IRQHandler(void)                	                   // 串口1中断服务程序
{

} 
void DMA1_Channel4_IRQHandler(void)  
{  
    DMA_ClearBitState(DMA1_FLAG_TC4);
     
      
    
    DMA_Enable(DMA1_CHANNEL4, DISABLE);
} 

/******************************************************************
  * @函数名称：   UartPrintf
  * @函数说明：   使用DMA输出打印信息
  * @输入参数：   无
  * @输出参数：   无       
  * @返回参数：   无              
  * @修改记录：   
******************************************************************/
TypeState UartPrintf(char *fmt, ...)
{
    char buff[256];
    int printed;
    uint32_t remaining_length;
    
    __va_list args;
    
    __va_start(args, fmt);
    
    printed = vsprintf(buff, fmt, args);        //转换数据
    
    __va_end(args);
    
    remaining_length = DMA_GetCurrDataCounter(DMA1_CHANNEL4);   //读取DMA目前指针位置，计算剩余多少数据没输出
    
    
    if ((remaining_length + printed) > 256)        //缓存满了，存不下，返回错误
    {
        return ERROR;
    }
    DMA_Enable(DMA1_CHANNEL4, DISABLE);             //把DMA停下了

    if (remaining_length > 0 && Uart_DMA_TX.data_length > remaining_length)
    {
        memcpy(UartSendBuff, &UartSendBuff[Uart_DMA_TX.data_length - remaining_length], remaining_length);      //把本次需要打印的信息粘贴到剩余信息的末尾，并且把缓存内已输出的信息去除
    }
    
    memcpy(&UartSendBuff[remaining_length], buff, printed);
    
    Uart_DMA_TX.data_length = printed + remaining_length;           //还需要输出的信息长度
    
    DMA_SetCurrDataCounter(DMA1_CHANNEL4, printed + remaining_length);      //设置DMA输出长度 
    
    DMA_Enable(DMA1_CHANNEL4, ENABLE); 
    
    return  SUCCESS;
}

	
#endif
