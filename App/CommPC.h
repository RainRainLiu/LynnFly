#ifndef _COMM_PC_H
#define _COMM_PC_H

#include "stm32f10x.h"


typedef struct 
{
    float ROOL;
    float PITCH;
    float THROTTLE;
    float YAW;
}RCData_Type;

extern RCData_Type RC_DATA;





void ANO_DT_Send_RCData(u16 thr,u16 yaw,u16 rol,u16 pit);
void ANO_DT_Send_Status(float angle_rol, float angle_pit, float angle_yaw, s32 alt, u8 fly_model, u8 armed);
#endif

