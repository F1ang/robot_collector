//
// Created by Hacker on 2024/4/28.
//

#ifndef BSP_USART_H
#define BSP_USART_H


#include "main.h"
#include "usart.h"

#define BUFFER_SIZE  22

typedef struct {
	float gyro_x,gyro_y,gyro_z; 
	float yaw,roll,pitch;
} imu_wit;

extern void USART_Init(void);
extern bool USART_Send_Pack(UART_HandleTypeDef *huart, short * WriteData, unsigned char MessageLenth, unsigned int MessageCode);
extern unsigned char CalCrc(unsigned char * VectorData, unsigned short len);

extern void Wit_imu_data(void);
extern imu_wit imu_wit_data;
extern uint8_t rx_buffer[22];
extern uint32_t rx_len;

#endif



