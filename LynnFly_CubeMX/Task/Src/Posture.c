#include "Posture.h"
#include "MPU6050.h"
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
static void Posture_Loop(void const * argument)
{
    uint8_t devID;
    while(1)
    {
        osDelay(1000);
        devID = MPU6050_GetDeviceID();
        osPrintf("MPU6050 Device id %x\r\n", devID);
        //DBG_PRINTF
    }
}


void Posture_Start(void)
{
    
    osThreadDef(PostureTask, Posture_Loop, osPriorityNormal, 1, 128);
    osThreadCreate(osThread(PostureTask), NULL);
    
}



