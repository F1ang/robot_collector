//
// Created by Hacker on 2024/4/28.
//
#include "bsp_usart.h"
#include "bsp_cmd.h"

#define LOG_TAG    "USART"
#include "bsp_log.h"

// 消息头/消息尾定义
const unsigned char HEAD[2] = {0x55, 0xaa};
const unsigned char END[2] = {0x0d, 0x0a};

union SerialMessageUnion
{
    short data;
    unsigned char data_transmit[2];
}SendUnion, ReceiveUnion;

u8 USART_Debug_RX_Buffer[1];//HAL库使用的串口接收缓冲
u8 USART_Robot_RX_Buffer[1];//HAL库使用的串口接收缓冲
static uint32_t USART_Robot_RX_Count=0;       //接收计数器
static uint8_t USART_Robot_RX_Package[255];     //接收缓冲，数据内容小于等于32Byte

u8 crc_t;
short MessageData[255] = {0};

int16_t robot_target_speed[3] = {0,0,0};  // X Y Yaw
int16_t robot_params[2] = {0,0};

/*-------------------- log pack的使用printf() --------------------*/
int fputc(int c, FILE *stream)
{
    HAL_UART_Transmit(&huart1, (unsigned char *)&c, 1, 1000);
    return 1;
}

uint8_t ch_r;
int fgetc(FILE * F)
{
    HAL_UART_Receive(&huart1,&ch_r,1,0xffff);//接收
    return ch_r;
}
/*-------------------- log pack的使用printf() --------------------*/

/*-------------------- Clion使用printf() --------------------*/
//#ifdef __GNUC__
//#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
//#else
//#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
//#endif

//PUTCHAR_PROTOTYPE
//{
//    HAL_UART_Transmit(&huart1, (uint8_t *) &ch, 1, 0xFFFF);
//    return ch;
//}
/*-------------------- Clion使用printf() --------------------*/

// 8位循环冗余校验计算函数
unsigned char CalCrc(unsigned char * VectorData, unsigned short len)
{
    unsigned char crc;
    crc = 0Xff;
    while(len--)
    {
        crc ^= (*VectorData++);
        for (int i = 0; i < 8; i++)
        {
            if(crc&0x80)
                crc = (crc << 1) ^ 0x1d;
            else
                crc <<= 1;
        }
    }
    //printf("crc = %d\r\n" , crc);
    return crc;
}

/* WIT IMU校验 */
u16 WITIMU_SUMCRC(u8 *data, u8 len)
{
    u16 temp;

    for (u8 i = 0; i < len; i++)
    {
        temp += data[i];
    }
    
    return temp;
}

uint8_t rx_buffer[22]={0};  //接收数据缓存数组

imu_wit imu_wit_data;  

uint32_t rx_len = 0;

void Wit_imu_data(void)
{
	//u16 crc = 0;
	//crc = WITIMU_SUMCRC(rx_buffer, 10);
	//if (crc == rx_buffer[10]){
		if (rx_buffer[1] == 0x52) {
			imu_wit_data.gyro_x = (s16)(((s16)rx_buffer[3] << 8) | rx_buffer[2]) / 32768.0f * 2000.0f;
			imu_wit_data.gyro_y = (s16)(((s16)rx_buffer[5] << 8) | rx_buffer[4]) / 32768.0f * 2000.0f;
			imu_wit_data.gyro_z = (s16)(((s16)rx_buffer[7] << 8) | rx_buffer[6]) / 32768.0f * 2000.0f;
		}
		if (rx_buffer[12] == 0x53) {
//			imu_wit_data.roll = (s16)(((s16)rx_buffer[14] << 8) | rx_buffer[13])  / 32768.0f * 16.0f;
//			imu_wit_data.pitch = (s16)(((s16)rx_buffer[16] << 8) | rx_buffer[15]) / 32768.0f * 16.0f;
//			imu_wit_data.yaw = (s16)(((s16)rx_buffer[18] << 8) | rx_buffer[17])   / 32768.0f * 16.0f;
				imu_wit_data.roll = (s16) (((s16)rx_buffer[14] << 8) | rx_buffer[13])  / 32768.0f * 160.0f;
				imu_wit_data.pitch = (s16) (((s16)rx_buffer[16] << 8) | rx_buffer[15]) / 32768.0f * 160.0f;
				imu_wit_data.yaw = (s16) (((s16)rx_buffer[18] << 8) | rx_buffer[17])   / 32768.0f * 160.0f;
		}
	//}
}

// 消息格式:
// 消息头1 消息头2 消息长度 消息码 [消息内容] 校验码 消息尾1 消息尾2
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance==USART1)
    {
        //换行符为0d0a
        if(USART_Debug_RX_Buffer[0] == 0x0d)
        {

        }
        else if(USART_Debug_RX_Buffer[0] == 0x0a)
        {
            CMD_Flag = 1;
        }
        else
        {
            CMD_Buffer[CMD_Buffer_Count] = USART_Debug_RX_Buffer[0];
            CMD_Buffer_Count++;
        }
        HAL_UART_Receive_IT(&huart1, (uint8_t *)USART_Debug_RX_Buffer, 1);  // 重新开启接收中断
    }

    if (huart->Instance==USART2)
    {

    }

}

// 串口发送函数
bool USART_Send_Pack(UART_HandleTypeDef *huart, short *WriteData, unsigned char MessageLenth, unsigned int MessageCode)
{
    // 计算发送消息长度
    unsigned char SendLength = (unsigned int)(2 * MessageLenth) + 7;

    // 发送消息缓冲区(变长数组)
    unsigned char * SendBuf = malloc(SendLength * sizeof(unsigned char));

    // 消息头 SendBuf[0] SendBuf[1]
    for(int i = 0; i < 2; i++)
    {
        SendBuf[i] = HEAD[i];
    }

    // 消息长度 SendBuf[2]
    SendBuf[2] =  SendLength;

    // 消息码 SendBuf[3]
    SendBuf[3] = (unsigned char)MessageCode;

    // 消息内容 SendBuf[4] ~ SendBuf[4 + 2*MessageLenth - 1]
    for(unsigned int i = 0; i < (unsigned int)MessageLenth; i++)
    {
        SendUnion.data = WriteData[i]; //更新消息Union内容
        for(int j = 0; j < 2; j++)
        {
            //送入缓冲区
            SendBuf[i * 2 + 4 + j] = SendUnion.data_transmit[j];  // u16->u8
        }
    }

    // 校验码计算 SendBuf[2*MessageLenth + 4]
    SendBuf[2*MessageLenth + 4] = CalCrc(SendBuf, 2*MessageLenth + 4); //校验MessageLenth消息+2消息头+1消息长度+1消息码=MessageLenth + 4

    // 消息尾 SendBuf[MessageLenth + 5] SendBuf[MessageLenth + 6]
    SendBuf[2*MessageLenth + 5] = END[0];
    SendBuf[2*MessageLenth + 6] = END[1];

    // 串口发送消息
    HAL_UART_Transmit(huart, (uint8_t *)SendBuf, SendLength, 100);

    //消息Union清零
    SendUnion.data = 0;

    //释放内存
    free(SendBuf);

    return true;
}

void USART_Init(void)
{
    HAL_UART_Receive_IT(&huart1, (uint8_t *)USART_Debug_RX_Buffer, 1);  // 开启接收中断
		//HAL_UART_Receive_IT(&huart2, (uint8_t *)USART_Robot_RX_Buffer, 1);  // 开启接收中断

		__HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE); //使能IDLE中断
		HAL_UART_Receive_DMA(&huart2,rx_buffer,BUFFER_SIZE);
	
    //LOG_I("USART Init Success\r\n");
}




