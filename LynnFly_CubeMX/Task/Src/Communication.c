
#include "Communication.h"
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
static void COMM_Loop(void const * argument)
{
    while(1)
    {
        osDelay(1000);
        //DBG_PRINTF("COMM Loop\r\n");
    }
}

/******************************************************************
  * @函数说明：   通讯任务启动
  * @输入参数：   标准printf输出
  * @输出参数：   无
  * @返回参数：   无             
  * @修改记录：   ----
******************************************************************/
void COMM_Start(void)
{
    osThreadDef(CommunicationTask, COMM_Loop, osPriorityNormal, 0, 128);
    osThreadCreate(osThread(CommunicationTask), NULL);

}



