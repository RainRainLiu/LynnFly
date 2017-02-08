#include "Battery.h"
#include <stm32f10x_dma.h>
#include <stm32f10x_adc.h>
#include "delay.h"
#define ADCCH_NUM  						1         				//要使用的ADC通道数

volatile uint16_t ad_value_real[ADCCH_NUM]	= {0};			// 实时值
volatile uint16_t ad_value_buf1[ADCCH_NUM]	= {0};			// 一阶滤波
volatile uint16_t ad_value_buf2[ADCCH_NUM]	= {0};			// 二级滤波
volatile uint16_t ad_value_buf3[ADCCH_NUM]	= {0};			// 一阶滤波
volatile uint16_t ad_value_buf4[ADCCH_NUM]	= {0};			// 二级滤波

void DmaAdc_Init()
{
	DMA_InitTypeDef dma;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

	DMA_DeInit(DMA1_Channel1);
	dma.DMA_PeripheralBaseAddr  = (uint32_t)&(ADC1->DR);     		//DMA对应的外设基地址
	dma.DMA_MemoryBaseAddr      = (uint32_t)&ad_value_real;       	//内存存储基地址
	dma.DMA_DIR                 = DMA_DIR_PeripheralSRC;			//DMA的转换模式为SRC模式，由外设搬移到内存
	dma.DMA_M2M                 = DMA_M2M_Disable;      			//M2M模式禁用
	dma.DMA_PeripheralDataSize  = DMA_PeripheralDataSize_HalfWord;	//定义外设数据宽度为16位
	dma.DMA_MemoryDataSize      = DMA_MemoryDataSize_HalfWord;  	//DMA搬移数据尺寸，HalfWord就是为16位    
	dma.DMA_BufferSize          = ADCCH_NUM;                       	//DMA缓存大小CH_NUM个
	dma.DMA_MemoryInc           = DMA_MemoryInc_Enable;         	//接收一次数据后，目标内存地址后移
	dma.DMA_PeripheralInc       = DMA_PeripheralInc_Disable;    	//接收一次数据后，设备地址禁止后移
	dma.DMA_Mode                = DMA_Mode_Circular;            	//转换模式，循环缓存模式。
	dma.DMA_Priority            = DMA_Priority_High;            	//DMA优先级高
	DMA_Init(DMA1_Channel1,&dma); 
	DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);
	DMA_ClearITPendingBit(DMA_IT_TC);
	DMA_ClearFlag(DMA1_FLAG_TC1);
}

// DMA1中断配置
void DmaIT_config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure; 

	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

// DMA读取数据完成中断
void DMA1_Channel1_IRQHandler(void)     
{
	uint8_t i;

	DMA_ClearFlag(DMA1_FLAG_TC1);
	DMA_ClearITPendingBit(DMA_IT_TC);
	for (i = 0; i < ADCCH_NUM; i++)
	{   
		ad_value_buf1[i] = (ad_value_real[i] + ad_value_buf1[i]) >> 1;
		ad_value_buf2[i] = (ad_value_buf1[i] + ad_value_buf2[i]) >> 1;
		ad_value_buf3[i] = (ad_value_buf2[i] + ad_value_buf3[i]) >> 1;
		ad_value_buf4[i] = (ad_value_buf3[i] + ad_value_buf4[i]) >> 1;
				
	}
}
// ADC初始化
void Adc_init(void)
{
	ADC_InitTypeDef adc;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);                       		// 设置ad时钟为72/6=12M,ad最大时钟频率为14M

	ADC_DeInit(ADC1);
	adc.ADC_Mode                = ADC_Mode_Independent;      		// 工作在独立模式
	adc.ADC_ContinuousConvMode  = ENABLE;                    		// 连续转换模式
	adc.ADC_DataAlign           = ADC_DataAlign_Right;       		// 数据右对齐
	adc.ADC_ExternalTrigConv    = ADC_ExternalTrigConv_None; 		// 不使用外部触发
	adc.ADC_NbrOfChannel        = ADCCH_NUM ;                		// 转换5个通道
	adc.ADC_ScanConvMode        = ENABLE;                    		// 连续转换模式
	ADC_Init(ADC1,&adc);		
    
    //ADC_TempSensorVrefintCmd(ENABLE); //开启内部温度传感器

	//设定指定的ADC通道组,设置转化顺序和采样时间
    ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 1, ADC_SampleTime_239Cycles5);


	
	//开启adc的DMA
	ADC_DMACmd(ADC1,ENABLE);
	//使能ADC1
	ADC_Cmd(ADC1, ENABLE);
	
	delay_ms(10);
	/* Enable ADC1 reset calibaration register */   
	ADC_ResetCalibration(ADC1);  					// 重新校准
	/* Check the end of ADC1 reset calibration register */
	while(ADC_GetResetCalibrationStatus(ADC1));  	// 等待重新校准完成
	/* Start ADC1 calibaration */
	ADC_StartCalibration(ADC1);  					// 开始校准
	/* Check the end of ADC1 calibration */
	while(ADC_GetCalibrationStatus(ADC1));       	// 等待校准完成
	ADC_TempSensorVrefintCmd(ENABLE);

}

// 启动adc,开始转换
void AdcStart(void)
{
    /* Start ADC1 Software Conversion */ 
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);  		//连续转换开始，ADC通过DMA方式不断的更新RAM区。 
    DMA_Cmd(DMA1_Channel1,ENABLE); 
    DmaIT_config();
}

// 关闭ADC
void AdcStop(void)
{
// 	DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, DISABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, DISABLE);
	ADC_Cmd(ADC1, DISABLE);
}

//获取ADC转换后的值
uint16_t AdcGetValue(uint8_t adc_Channel)
{	
	return ad_value_buf4[adc_Channel];
}

// 函数名称：ADC配置
// 函数功能：ADC的配置和启动
void AdcConfig(void)
{
	Adc_init();
	AdcStart();
}

/*
//实例化一个电压信息结构体
Bat_Typedef Battery;


//初始化电池检测ADC
//开启ADC1的通道8	
//BatteryCheck---->PB0
void BatteryCheckInit()
{
  
 //先初PB0为模拟输入
  RCC->APB2ENR|=1<<3;    //使能PORTB口时钟 
  GPIOB->CRL&=0XFFFFFFF0;//PB0	anolog输入
	//通道8	 
	RCC->APB2ENR|=1<<9;    //ADC1时钟使能	  
	RCC->APB2RSTR|=1<<9;   //ADC1复位
	RCC->APB2RSTR&=~(1<<9);//复位结束	    
	RCC->CFGR&=~(3<<14);   //分频因子清零	
	//SYSCLK/DIV2=12M ADC时钟设置为12M,ADC最大时钟不能超过14M!
	//否则将导致ADC准确度下降! 
	RCC->CFGR|=2<<14;      	 
	ADC1->CR1&=0XF0FFFF;   //工作模式清零
	ADC1->CR1|=0<<16;      //独立工作模式  
	ADC1->CR1&=~(1<<8);    //非扫描模式	  
	ADC1->CR2&=~(1<<1);    //单次转换模式
	ADC1->CR2&=~(7<<17);	   
	ADC1->CR2|=7<<17;	     //软件控制转换  
	ADC1->CR2|=1<<20;      //使用用外部触发(SWSTART)!!!	必须使用一个事件来触发
	ADC1->CR2&=~(1<<11);   //右对齐	 
	ADC1->CR2|=1<<23;      //使能温度传感器

	ADC1->SQR1&=~(0XF<<20);
	ADC1->SQR1&=0<<20;     //1个转换在规则序列中 也就是只转换规则序列1 			   
	//设置通道1的采样时间
	ADC1->SMPR2&=~(7<<3);  //通道1采样时间清空	  
 	ADC1->SMPR2|=7<<3;     //通道1  239.5周期,提高采样时间可以提高精确度	 

 	ADC1->SMPR1&=~(7<<18);  //清除通道16原来的设置	 
	ADC1->SMPR1|=7<<18;     //通道16  239.5周期,提高采样时间可以提高精确度	 

	ADC1->CR2|=1<<0;	   //开启AD转换器	 
	ADC1->CR2|=1<<3;       //使能复位校准  
	while(ADC1->CR2&1<<3); //等待校准结束 			 
  //该位由软件设置并由硬件清除。在校准寄存器被初始化后该位将被清除。 		 
	ADC1->CR2|=1<<2;        //开启AD校准	   
	while(ADC1->CR2&1<<2);  //等待校准结束
	//该位由软件设置以开始校准，并在校准结束时由硬件清除  
  
  Battery.BatReal = 3.95;//单位为v 电池实际电压  校准电压时修改
  Battery.ADinput = 1.98;//单位为v R15和R17连接处电压 校准电压时修改
  Battery.ADRef   = 3.26;//单位为v 单片机供电电压   校准电压时修改
  Battery.Bat_K   = Battery.BatReal/Battery.ADinput;//计算电压计算系数
	Battery.overDischargeCnt = 0;
  
  printf("Batter voltage AD init ...\r\n");
  
}


//获得ADC值
//ch:通道值 0~16
//返回值:转换结果
u16 Get_Adc(u8 ch)   
{
	//设置转换序列	  		 
	ADC1->SQR3&=0XFFFFFFE0;//规则序列1 通道ch
	ADC1->SQR3|=ch;		  			    
	ADC1->CR2|=1<<22;       //启动规则转换通道 
	while(!(ADC1->SR&1<<1));//等待转换结束	 	   
	return ADC1->DR;		    //返回adc值	
}

//获取通道ch的转换值，取times次,然后平均 
//ch:通道编号
//times:获取次数
//返回值:通道ch的times次转换结果平均值
u16 Get_Adc_Average(u8 ch,u8 times)
{
	u32 temp_val=0;
	u8 t;
	for(t=0;t<times;t++)
	{
		temp_val+=Get_Adc(ch);
	}
	return temp_val/times;
} 

//得到ADC采样内部温度传感器的温度值
//返回值3位温度值 XXX*0.1C	 
int Get_Temp(void)
{				 
	u16 temp_val=0;
	u8 t;
	float temperate;   
	for(t=0;t<20;t++)//读20次,取平均值
	{
		temp_val+=Get_Adc(16);//温度传感器为通道16
	}
	temp_val/=20;
	temperate=(float)temp_val*(3.3/4096);//得到温度传感器的电压值
	temperate=(1.43-temperate)/0.0043+25;//计算出当前温度值	 
	temperate*=10;//扩大十倍,使用小数点后一位
	return (int)temperate;	 
}


//返回电池电压AD值
int GetBatteryAD()
{
 return Get_Adc_Average(8,5);
}


*/
//检测电池电压
void BatteryCheck(void)
{
//		Battery.BatteryAD  = GetBatteryAD();            //电池电压检测  
//		Battery.BatteryVal = Battery.Bat_K * (Battery.BatteryAD/4096.0) * Battery.ADRef;//实际电压 值计算	
//	
//	  if(FLY_ENABLE){
//			//处于电机开启等飞行状态，在过放电压值（BAT_OVERDIS_VAL）以上0.03v以上，开始报警
//			if(Battery.BatteryVal <= (BAT_OVERDIS_VAL + 0.03)){
//					Battery.alarm=1;
//			}else{
//					Battery.alarm=0;
//			}
//			
//			//过放保护，Battery overdischarge protect
//			if(Battery.BatteryVal <= BAT_OVERDIS_VAL){
//				Battery.overDischargeCnt++;
//				
//				if(Battery.overDischargeCnt > 8){
//					altCtrlMode=LANDING;
//					rcData[0]=1500;rcData[1]=1500;rcData[2]=1500;rcData[3]=1500;
//				}
//			}else{
//				Battery.overDischargeCnt = 0;
//			}
//		}else{
//			if((Battery.BatteryVal < BAT_ALARM_VAL)&&(Battery.BatteryVal > BAT_CHG_VAL)){	//低于3.7v 且大于充电检测电压 BAT_CHG_VAL
//				Battery.alarm=1;
//			}else{
//				Battery.alarm=0;
//			}
//		}
//		
//		if(Battery.BatteryVal < BAT_CHG_VAL){ //on charge
//			Battery.chargeSta = 1; 
//			BT_off();
//		}else{
//			Battery.chargeSta = 0;
//		}

}



