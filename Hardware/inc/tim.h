#ifndef _tim_H_
#define _tim_H_
#include "stm32f10x.h"


extern uint32_t TimeCount;

void TIM4_Init(int clock,int Preiod);//用于监测系统
void TIM3_Init(char clock,int Preiod);//定时器3的初始化
void TimerNVIC_Configuration(void);//定时器中断向量表配置

#endif

