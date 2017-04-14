
#include "Communication.h"
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"
#include "usart.h"

/******************************************************************
  * @����˵����   ͨѶѭ������
  * @���������   
  * @���������   ��
  * @���ز�����   ��             
  * @�޸ļ�¼��   ----
******************************************************************/
static void COMM_Loop(void const * argument)
{
    while(1)
    {
        osDelay(1000);
        //DBG_PRINTF("COMM Loop\r\n");
    }
}

/******************************************************************
  * @����˵����   ͨѶ��������
  * @���������   ��׼printf���
  * @���������   ��
  * @���ز�����   ��             
  * @�޸ļ�¼��   ----
******************************************************************/
void COMM_Start(void)
{
    osThreadDef(CommunicationTask, COMM_Loop, osPriorityNormal, 0, 128);
    osThreadCreate(osThread(CommunicationTask), NULL);

}


