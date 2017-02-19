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
  * @�������ƣ�   uart_dma_init
  * @����˵����   ����DMA��ʼ������Ӧͨ��DMA4��USART1
  * @���������   ��
  * @���������   ��       
  * @���ز�����   ��             
  * @�޸ļ�¼��   
******************************************************************/
void uart_dma_init(void)
{
    
    DMA_InitTypeDef DMA_InitStructure;
 
    //����DMAʱ��  
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1 | RCC_AHBPeriph_DMA2, ENABLE);  

      
    DMA_DeInit(DMA1_Channel4);                                              //DMA1ͨ��4����
                                                                            
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART1->DR);           //�����ַ  
                                                                             
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)UartSendBuff;           //�ڴ��ַ  
                                                                             
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;                       //dma���䷽����  
                                                                             
    DMA_InitStructure.DMA_BufferSize = 0;                                  //����DMA�ڴ���ʱ�������ĳ���  
                                                                             
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;         //����DMA���������ģʽ��һ������  
                                                                             
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                  //����DMA���ڴ����ģʽ  
                                                                             
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  //���������ֳ�  
                                                                              
    DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_Byte;      //�ڴ������ֳ�  
                                                                             
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                            //����DMA�Ĵ���ģʽ  
                                                                             
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;                    //����DMA�����ȼ���  
                                                                             
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                             //����DMA��2��memory�еı����������
    
    
    DMA_Init(DMA1_Channel4,&DMA_InitStructure);  

    
    USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE);    //����DMA��ʽ����  

}
/******************************************************************
  * @�������ƣ�   Uart_init
  * @����˵����   u32 bound  ������
  * @���������   ��
  * @���������   ��       
  * @���ز�����                
  * @�޸ļ�¼��   
******************************************************************/
void Uart_init(u32 bound)
{
    GPIO_InitTypeDef    GPIO_InitStructure;
    USART_InitTypeDef   USART_InitStructure;
    NVIC_InitTypeDef    NVIC_InitStructure;
    
    //RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
 	USART_DeInit(USART1);  //��λ����1
    
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
    
    USART_Init(USART1, &USART_InitStructure);                    // ��ʼ������
    
    USART_Cmd(USART1, ENABLE); 
    
    uart_dma_init();
    
}



/******************************************************************
  * @�������ƣ�   Uart_SendByte
  * @����˵����   �����ַ������
  * @���������   USART_TypeDef* USARTx,     ���ں� 
                  uint8_t data,            Ŀ������
  * @���������   ��       
  * @���ز�����   ��             
  * @�޸ļ�¼��   
******************************************************************/
void Uart_SendByte(USART_TypeDef* USARTx, uint8_t data)
{
    USART_ClearFlag(USARTx,USART_FLAG_TC);
    
    USART_SendData(USARTx, data);
    
    while(USART_GetFlagStatus(USARTx,USART_FLAG_TC) == RESET);
}

/******************************************************************
  * @�������ƣ�   Uart_SendString
  * @����˵����   �����ַ������
  * @���������   USART_TypeDef* USARTx,     ���ں� 
                 uint8_t *pBuff,            Ŀ������
                 uint32_t nLength           ���ݳ���
  * @���������   ��       
  * @���ز�����   ��             
  * @�޸ļ�¼��   
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
  * @�������ƣ�   UartDMA_SendString
  * @����˵����   DMA���������Ϣ���������򲻽����µ�����
  * @���������   UART_DMA_SEND_HANDLE_T *sendHandle, ʹ�õ�DMA���
                 uint8_t *pBuff,    Ŀ������
                 uint32_t nLength   ���ݳ���
  * @���������   ��ǰҪ���͵������ݳ���       
  * @���ز�����   �����Ƿ�ɹ����������򷵻�ʧ�ܣ�             
  * @�޸ļ�¼��   
******************************************************************/
ErrorStatus UartDMA_SendString(UART_DMA_SEND_HANDLE_T *sendHandle, uint8_t *pBuff, uint32_t nLength)
{
    uint32_t remaining_length;
    
    remaining_length = DMA_GetCurrDataCounter(sendHandle->DMAy_Channelx);   //��ȡDMAĿǰָ��λ�ã�����ʣ���������û���
    
    
    if ((remaining_length + nLength) > UART_DMA_CACHE_SIZE)        //�������ˣ��治�£����ش���
    {
        return ERROR;
    }
    DMA_Cmd(sendHandle->DMAy_Channelx, DISABLE);             //��DMAͣ����

    if (remaining_length > 0 && sendHandle->dataLength > remaining_length)  //�����ڻ���δ���͵�����
    {
        memcpy(UartSendBuff, &UartSendBuff[sendHandle->dataLength - remaining_length], remaining_length);      //�ѱ�����Ҫ��ӡ����Ϣճ����ʣ����Ϣ��ĩβ�����Ұѻ��������������Ϣȥ��
    }
    
    memcpy(&UartSendBuff[remaining_length], pBuff, nLength);
    
    sendHandle->dataLength = nLength + remaining_length;           //����Ҫ�������Ϣ����
    
    DMA_SetCurrDataCounter(sendHandle->DMAy_Channelx, nLength + remaining_length);      //����DMA������� 
    
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
    
    printed = vsprintf(buff, fmt, args);        //ת������
    
    __va_end(args);
    
    
    return UartDMA_SendString(sendHandle, (uint8_t *)buff, printed);
}






