#include "main.h"
#include "bsp_cmd.h"
#include <cstring>
#include <stdlib.h>
#include "bsp_adc.h"

#define SCB_AIRCR (*(volatile unsigned long *)0xE000ED0C) /* Reset control Address Register */
#define SCB_RESET_VALUE 0x05FA0004                        /* Reset value, write to SCB_AIRCR can reset cpu */

extern foc_handler foc_data_handler;

void split(char *src, const char *separator, char **dest, int *num)
{
    char *pNext;
    // 记录分隔符数量
    int count = 0;
    // 原字符串为空
    if (src == NULL || strlen(src) == 0)
        return;
    // 未输入分隔符
    if (separator == NULL || strlen(separator) == 0)
        return;
    /*

    */
    char *strtok(char *str, const char *delim);
    // 获得第一个由分隔符分割的字符串
    pNext = strtok(src, separator);
    while (pNext != NULL)
    {
        // 存入到目的字符串数组中
        *dest++ = pNext;
        ++count;
        pNext = strtok(NULL, separator);
    }
    *num = count;
}

char CMD_Buffer[50] = {0};
int CMD_Buffer_Count = 0;
int CMD_Flag = 0;

// 接收串口中断
void cmd(void)
{
    if (CMD_Flag)
    {
        CMD_Flag = 0;
    }
    else
    {
        return;
    }

    printf("cmd> ");
    if (!strcmp(CMD_Buffer, "hi"))
    {
        printf("hi there~\r\n");
    }
    else if (!strcmp(CMD_Buffer, "reboot"))
    {
        reboot();
    }
    else if (!strcmp(CMD_Buffer, "help"))
    {
        help();
    }
    else if (!strcmp(CMD_Buffer, "debug_on"))
    {
        debug_on();
    }
    else if (!strcmp(CMD_Buffer, "debug_off"))
    {
        debug_off();
    }
    else if (!strcmp(CMD_Buffer, "show_encoder"))
    {
        show_encoder();
    }
    else if (!strcmp(CMD_Buffer, "show_foc"))
    {
        show_foc();
    }
    else if (!strcmp(CMD_Buffer, "set_uqud"))
    {
        set_uqud();
    }
    else if (!strcmp(CMD_Buffer, "set_pos"))
    {
        set_pos();
    }
    else if (strstr(CMD_Buffer, "set_pid"))
    {
        set_pid();
    }
    else if (strstr(CMD_Buffer, "set_speed"))
    {
        set_speed();
    }
    else
    {
        printf("unknown cmd '%s'\r\n", CMD_Buffer);
        help();
    }

    // 缓存区清零
    for (int i = CMD_Buffer_Count; i >= 0; i--)
    {
        CMD_Buffer[i] = 0;
    }
    CMD_Buffer_Count = 0;
}

void reboot(void)
{
    printf("\r\n^^^^^^^^^^^^^^^^^^^^^System Rebooting^^^^^^^^^^^^^^^^^^^^^\r\n\r\n\r\n\r\n");
    SCB_AIRCR = SCB_RESET_VALUE;
}

void help(void)
{
    printf("Available commands: \r\n");
    printf("hi                   ----------     Say hi to me\r\n");
    printf("reboot               ----------     System reboot\r\n");
    printf("help                 ----------     Help list\r\n");
    printf("debug_on             ----------     Show debug messages\r\n");
    printf("debug_off            ----------     Don't show debug messages\r\n");
    printf("show_encoder         ----------     Show as5600\r\n");
    printf("show_foc             ----------     Show foc data\r\n");
    printf("set_uqud             ----------     Set uq ud value\r\n");
    printf("set_pos              ----------     Set position loop\r\n");
    printf("set_pid              ----------     Set motor pid, format: [set_pid P I D] \r\n");
    printf("set_speed            ----------     Set speed loop \r\n");
}

void debug_on(void)
{
    printf("debug_on, elog_set_filter_lvl: ELOG_LVL_DEBUG\r\n");
}

void debug_off(void)
{
    printf("debug_off, elog_set_filter_lvl: ELOG_LVL_INFO\r\n");
}

void show_encoder(void)
{
    printf("%.2f %.2f %.2f\r\n", foc_data_handler.ia, foc_data_handler.ib, foc_data_handler.ic);
}

void show_foc(void)
{
}
void set_uqud(void)
{
}

void set_pos(void)
{
}

void set_pid(void)
{
}

void set_speed(void)
{
    char *buf[4] = {0};
    int num = 0;
    split(CMD_Buffer, " ", buf, &num);

    foc_data_handler.uq = (int16_t)(atof(buf[2]));

    // 上位机参数:5 0 70
    // foc_data_handler.pos_loop.target_pos = (int16_t)(atof(buf[1])); // 单位rad
    // foc_data_handler.pos_loop.kp = -(float)(atof(buf[3])) / 10;

    // 上位机参数:1 0 -8
    // foc_data_handler.speed_loop.target_speed = (int16_t)(atof(buf[1])); // 单位rad/s    1 0 -8
    // foc_data_handler.speed_loop.kp = -(float)(atof(buf[3])) / 10;

    // 上位机参数:set_speed 3 -8 0.002
    // bsp_adc.iq_ref = (float)(atof(buf[1])); // 单位0.1A
    // bsp_adc.kp = (float)(atof(buf[2])) / 10;
    // bsp_adc.ki = (float)(atof(buf[3])) / 10;

    printf("\t[%.2f]\t[%.2f]\t[%.1f]\t\r\n",
           atof(buf[1]),
           atof(buf[2]),
           atof(buf[3]));
}
