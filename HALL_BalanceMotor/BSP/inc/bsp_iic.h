//
// Created by Hacker on 2024/4/28.
//

#ifndef HALL_BALANCEMOTOR_BSP_IIC_H
#define HALL_BALANCEMOTOR_BSP_IIC_H

#include "main.h"

#define IIC_SCL    PBout(8)
#define IIC_SDA    PBout(9)
#define READ_SDA   PBin(9)

#define SDA_IN()  {GPIOB->CRH&=0XFFFFFF0F;GPIOB->CRH|=(u32)8<<4;}
#define SDA_OUT() {GPIOB->CRH&=0XFFFFFF0F;GPIOB->CRH|=(u32)3<<4;}

void IIC_Delay(void);
void IIC_Init(void);
void IIC_Start(void);
void IIC_Stop(void);
void IIC_Send_Byte(u8 txd);
u8 IIC_Read_Byte(unsigned char ack);
u8 IIC_Wait_Ack(void);
void IIC_Ack(void);
void IIC_NAck(void);

void IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 IIC_Read_One_Byte(u8 daddr,u8 addr);

#endif //HALL_BALANCEMOTOR_BSP_IIC_H
