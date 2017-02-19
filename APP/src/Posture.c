#include "Posture.h"
#include "MPU6050.h"



void Posture_Task(void *pvParameters)
{
    MPU6050_Check();
    MPU6050_initialize();
    while(1)
    {        
        //OS_Printf("tick\r\n");
        
        vTaskDelay(1000);
    }
}