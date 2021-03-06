#ifndef __IIC_H
#define __IIC_H
#include "sys.h"
#include "stm32f10x.h"


#define IIC_SDA_IO  GPIO_Pin_6 
#define IIC_SCL_IO  GPIO_Pin_7

#define IIC_SCL    PBout(7) //SCL
#define IIC_SDA    PBout(6) //SDA	 
#define READ_SDA   PBin(6)  //输入SDA 


#define SDA_IN()  {GPIOB->CRL&=0XF0FFFFFF;GPIOB->CRL|=0x08000000;}
#define SDA_OUT() {GPIOB->CRL&=0XF0FFFFFF;GPIOB->CRL|=0x03000000;}



//IIC所有操作函数
void IIC_Init(void);          //初始化IIC的IO口				 
void IIC_Start(void);			   	//发送IIC开始信号
void IIC_Stop(void);	  			//发送IIC停止信号
void IIC_Send_Byte(u8 txd);			//IIC发送一个字节
u8 IIC_Read_Byte(unsigned char ack);//IIC读取一个字节
u8 IIC_Wait_Ack(void); 				//IIC等待ACK信号
void IIC_Ack(void);					//IIC发送ACK信号
void IIC_NAck(void);				//IIC不发送ACK信号

void IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 IIC_Read_One_Byte(u8 daddr,u8 addr);	 
unsigned char I2C_Readkey(unsigned char I2C_Addr);

unsigned char I2C_ReadOneByte(unsigned char I2C_Addr,unsigned char addr);
unsigned char IICwriteByte(unsigned char dev, unsigned char reg, unsigned char data);
u8 IICwriteBytes(u8 dev, u8 reg, u8 length, u8* data);
u8 IICwriteBits(u8 dev,u8 reg,u8 bitStart,u8 length,u8 data);
u8 IICwriteBit(u8 dev,u8 reg,u8 bitNum,u8 data);
u8 IICreadBytes(u8 dev, u8 reg, u8 length, u8 *data);

#endif

//------------------End of File----------------------------







