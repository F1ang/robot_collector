#include "main.h"
#include "bsp_usart.h"
#include <stdlib.h>

#define LOG_TAG "USART"
#include "bsp_log.h"

extern UART_HandleTypeDef huart1;
uint8_t USART_Debug_RX_Buffer[1]; // HAL库使用的串口接收缓冲

extern char CMD_Buffer[];
extern int CMD_Buffer_Count;
extern int CMD_Flag;

int fputc(int c, FILE *stream)
{
    HAL_UART_Transmit(&huart1, (unsigned char *)&c, 1, 1000);
    return 1;
}

uint8_t ch_r;
int fgetc(FILE *F)
{
    HAL_UART_Receive(&huart1, &ch_r, 1, 0xffff); // 接收
    return ch_r;
}

// 消息格式:
// 消息头1 消息头2 消息长度 消息码 [消息内容] 校验码 消息尾1 消息尾2
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        // 换行符为0d0a
        if (USART_Debug_RX_Buffer[0] == 0x0d)
        {
            ;
        }
        else if (USART_Debug_RX_Buffer[0] == 0x0a)
        {
            CMD_Flag = 1;
        }
        else
        {
            CMD_Buffer[CMD_Buffer_Count] = USART_Debug_RX_Buffer[0];
            CMD_Buffer_Count++;
        }
        HAL_UART_Receive_IT(&huart1, (uint8_t *)USART_Debug_RX_Buffer, 1); // 重新开启接收中断
    }
}

void USART_Init(void)
{
    HAL_UART_Receive_IT(&huart1, (uint8_t *)USART_Debug_RX_Buffer, 1); // 开启接收中断
    LOG_I("USART Init Success\r\n");
}
