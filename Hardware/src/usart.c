/******************************************************************
                DMA ���ڴ�ӡ��������ʽ
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
    
	

	//Usart1 NVIC ����    
    NVIC_InitStructure.NVIC_IRQ = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQPreemptPriority = 5;
    NVIC_InitStructure.NVIC_IRQSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQEnable    = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	                           // ����ָ���Ĳ�����ʼ��VIC�Ĵ���
    
    
    
    USART_DeInit( USART1 );
    USART_InitStructure.USART_BRR           = bound;
    USART_InitStructure.USART_WL            = USART_WL_8B;
    USART_InitStructure.USART_STBits            = USART_STBITS_1;
    USART_InitStructure.USART_Parity                = USART_PARITY_RESET;
    USART_InitStructure.USART_HardwareFlowControl = USART_HARDWAREFLOWCONTROL_NONE;
    USART_InitStructure.USART_RxorTx                = USART_RXORTX_RX | USART_RXORTX_TX;
    USART_Init(USART1, &USART_InitStructure);
    
	USART_Init(USART1, &USART_InitStructure);                    // ��ʼ������
    
    USART_Enable(USART1, ENABLE); 
    uart_dma_init();
}


void uart_dma_init(void)
{
    NVIC_InitPara   NVIC_InitStructure;
    
    DMA_InitPara    DMA_InitStructure;
    
 
    //����DMAʱ��  
    //RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_DMA1, ENABLE);  
    
    RCC_AHBPeriphClock_Enable(RCC_AHBPERIPH_DMA1, ENABLE);
    
    
    NVIC_InitStructure.NVIC_IRQ = DMA1_Channel4_IRQn;           //DMA�����ж����� 
    NVIC_InitStructure.NVIC_IRQPreemptPriority = 3;
    NVIC_InitStructure.NVIC_IRQSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQEnable    = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	                           // ����ָ���Ĳ�����ʼ��VIC�Ĵ���
    
    
                                                                 
  
      
    DMA_DeInit(DMA1_CHANNEL4);                                              //DMA1ͨ��4����
                                                                            
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART1->DR);           //�����ַ  
                                                                             
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)UartSendBuff;           //�ڴ��ַ  
                                                                             
    DMA_InitStructure.DMA_DIR = DMA_DIR_PERIPHERALDST;//DMA_DIR_PeripheralDST;                       //dma���䷽����  
                                                                             
    DMA_InitStructure.DMA_BufferSize = 0;                                  //����DMA�ڴ���ʱ�������ĳ���  
                                                                             
    DMA_InitStructure.DMA_PeripheralInc = DMA_PERIPHERALINC_DISABLE; //DMA_PeripheralInc_Disable;         //����DMA���������ģʽ��һ������  
                                                                             
    DMA_InitStructure.DMA_MemoryInc = DMA_MEMORYINC_ENABLE;//DMA_MemoryInc_Enable;                  //����DMA���ڴ����ģʽ  
                                                                             
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PERIPHERALDATASIZE_BYTE;//DMA_PeripheralDataSize_Byte;  //���������ֳ�  
                                                                              
    DMA_InitStructure.DMA_MemoryDataSize = DMA_PERIPHERALDATASIZE_BYTE;//DMA_PeripheralDataSize_Byte;      //�ڴ������ֳ�  
                                                                             
    DMA_InitStructure.DMA_Mode = DMA_MODE_NORMAL;//DMA_Mode_Normal;                            //����DMA�Ĵ���ģʽ  
                                                                             
    DMA_InitStructure.DMA_Priority = DMA_PRIORITY_MEDIUM;//DMA_Priority_Medium;                    //����DMA�����ȼ���  
                                                                             
    //DMA_InitStructure.DMA_M2M = DMA_M//DMA_M2M_Disable;                             //����DMA��2��memory�еı����������
    DMA_InitStructure.DMA_MTOM = DMA_MEMTOMEM_DISABLE;
    
    DMA_Init(DMA1_CHANNEL4,&DMA_InitStructure);  
 
    DMA_INTConfig(DMA1_CHANNEL4, DMA_INT_TC, ENABLE);
    //����DMA��ʽ����
    USART_DMA_Enable(USART1, USART_DMAREQ_TX, ENABLE);

      


}

void USART1_IRQHandler(void)                	                   // ����1�жϷ������
{

} 
void DMA1_Channel4_IRQHandler(void)  
{  
    DMA_ClearBitState(DMA1_FLAG_TC4);
     
      
    
    DMA_Enable(DMA1_CHANNEL4, DISABLE);
} 

/******************************************************************
  * @�������ƣ�   UartPrintf
  * @����˵����   ʹ��DMA�����ӡ��Ϣ
  * @���������   ��
  * @���������   ��       
  * @���ز�����   ��              
  * @�޸ļ�¼��   
******************************************************************/
TypeState UartPrintf(char *fmt, ...)
{
    char buff[256];
    int printed;
    uint32_t remaining_length;
    
    __va_list args;
    
    __va_start(args, fmt);
    
    printed = vsprintf(buff, fmt, args);        //ת������
    
    __va_end(args);
    
    remaining_length = DMA_GetCurrDataCounter(DMA1_CHANNEL4);   //��ȡDMAĿǰָ��λ�ã�����ʣ���������û���
    
    
    if ((remaining_length + printed) > 256)        //�������ˣ��治�£����ش���
    {
        return ERROR;
    }
    DMA_Enable(DMA1_CHANNEL4, DISABLE);             //��DMAͣ����

    if (remaining_length > 0 && Uart_DMA_TX.data_length > remaining_length)
    {
        memcpy(UartSendBuff, &UartSendBuff[Uart_DMA_TX.data_length - remaining_length], remaining_length);      //�ѱ�����Ҫ��ӡ����Ϣճ����ʣ����Ϣ��ĩβ�����Ұѻ��������������Ϣȥ��
    }
    
    memcpy(&UartSendBuff[remaining_length], buff, printed);
    
    Uart_DMA_TX.data_length = printed + remaining_length;           //����Ҫ�������Ϣ����
    
    DMA_SetCurrDataCounter(DMA1_CHANNEL4, printed + remaining_length);      //����DMA������� 
    
    DMA_Enable(DMA1_CHANNEL4, ENABLE); 
    
    return  SUCCESS;
}

	
#endif
