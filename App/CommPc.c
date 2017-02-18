#include "CommPC.h"
#include "usart.h"
//数据拆分宏定义，在发送大于1字节的数据类型时，比如int16、float等，需要把数据拆分成单独字节进行发送
#define BYTE0(dwTemp)       ( *( (char *)(&dwTemp)      ) )
#define BYTE1(dwTemp)       ( *( (char *)(&dwTemp) + 1) )
#define BYTE2(dwTemp)       ( *( (char *)(&dwTemp) + 2) )
#define BYTE3(dwTemp)       ( *( (char *)(&dwTemp) + 3) )


RCData_Type RC_DATA;


void UartSendData(uint8_t data)
{
    while((USART1->SR&0X40)==0)
    {
        
    };//循环发送,直到发送完毕   
    USART1->DR = (u8)data;
}

void UartSendString(uint8_t *data, uint8_t cnt)
{
    uint8_t i = 0;
    
    
    for(i=0;i<cnt;i++)
    {
        UartSendData(*data);
        data++;
    }
}


void ANO_DT_Send_RCData(u16 thr,u16 yaw,u16 rol,u16 pit)
{
    u8 sum = 0;
    u8 _cnt=0;
    uint8_t data_to_send[50];
    
    data_to_send[_cnt++] = 0xAA;
    data_to_send[_cnt++] = 0xAA;
    data_to_send[_cnt++] = 0x03;
    data_to_send[_cnt++] = 0;
    data_to_send[_cnt++] = BYTE1(thr);
    data_to_send[_cnt++] = BYTE0(thr);
    data_to_send[_cnt++] = BYTE1(yaw);
    data_to_send[_cnt++] = BYTE0(yaw);
    data_to_send[_cnt++] = BYTE1(rol);
    data_to_send[_cnt++] = BYTE0(rol);
    data_to_send[_cnt++] = BYTE1(pit);
    data_to_send[_cnt++] = BYTE0(pit);
    data_to_send[_cnt++] = 0;
    data_to_send[_cnt++] = 0;
    data_to_send[_cnt++] = 0;
    data_to_send[_cnt++] = 0;
    data_to_send[_cnt++] = 0;
    data_to_send[_cnt++] = 0;
    data_to_send[_cnt++] = 0;
    data_to_send[_cnt++] = 0;
    data_to_send[_cnt++] = 0;
    data_to_send[_cnt++] = 0;
    data_to_send[_cnt++] = 0;
    data_to_send[_cnt++] = 0;

    data_to_send[3] = _cnt-4;
    
    
    for(u8 i=0;i<_cnt;i++)
    {
        sum += data_to_send[i];
    }
    data_to_send[_cnt++] = sum;
    
    UartSendString(data_to_send, _cnt);
}

void ANO_DT_Send_Status(float angle_rol, float angle_pit, float angle_yaw, s32 alt, u8 fly_model, u8 armed)
{
    u8 _cnt=0;
    vs16 _temp;
    vs32 _temp2 = alt;
    uint8_t data_to_send[50];
    
    data_to_send[_cnt++]=0xAA;
    data_to_send[_cnt++]=0xAA;
    data_to_send[_cnt++]=0x01;
    data_to_send[_cnt++]=0;
    
    _temp = (int)(angle_rol*100);
    data_to_send[_cnt++]=BYTE1(_temp);
    data_to_send[_cnt++]=BYTE0(_temp);
    _temp = (int)(angle_pit*100);
    data_to_send[_cnt++]=BYTE1(_temp);
    data_to_send[_cnt++]=BYTE0(_temp);
    _temp = (int)(angle_yaw*100);
    data_to_send[_cnt++]=BYTE1(_temp);
    data_to_send[_cnt++]=BYTE0(_temp);
    
    data_to_send[_cnt++]=BYTE3(_temp2);
    data_to_send[_cnt++]=BYTE2(_temp2);
    data_to_send[_cnt++]=BYTE1(_temp2);
    data_to_send[_cnt++]=BYTE0(_temp2);
    
    data_to_send[_cnt++] = fly_model;
    
    data_to_send[_cnt++] = armed;
    
    data_to_send[3] = _cnt-4;
    
    u8 sum = 0;
    for(u8 i=0;i<_cnt;i++)
        sum += data_to_send[i];
    data_to_send[_cnt++]=sum;
    
    UartSendString(data_to_send, _cnt);
}
