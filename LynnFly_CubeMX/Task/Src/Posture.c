#include "Posture.h"
#include "MPU6050.h"
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"
#include "usart.h"

/******************************************************************
  * @函数说明：   通讯循环处理
  * @输入参数：   
  * @输出参数：   无
  * @返回参数：   无             
  * @修改记录：   ----
******************************************************************/
static void Posture_Loop(void const * argument)
{
    MPU6050_SENSOR_DATA_T sensorData;
    
    if (MPU6050_initialize() == SUCCESS)
    {
        osPrintf("MPU6050 Initialize Sucess\r\n");
    }
    
    while(1)
    {
        osDelay(1000);
        sensorData = MPU6050_ReadSensor();

        osPrintf("acc  X %4.4f Y %4.4f Z %4.4f\r\n", sensorData.accX,sensorData.accY,sensorData.accZ);
        osPrintf("gyro X %4.4f Y %4.4f Z %4.4f\r\n", sensorData.gyroX,sensorData.gyroY,sensorData.gyroZ);
    }
}


void Posture_Start(void)
{
    
    osThreadDef(PostureTask, Posture_Loop, osPriorityNormal, 1, 128);
    osThreadCreate(osThread(PostureTask), NULL);
    
}



