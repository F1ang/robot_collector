#ifndef HALL_BALANCEMOTOR_BSP_CMD_H
#define HALL_BALANCEMOTOR_BSP_CMD_H

#include "main.h"
#include "bsp_foc.h"
#include "bsp_usart.h"

extern char CMD_Buffer[];
extern int CMD_Buffer_Count;
extern int CMD_Flag;

#define cmd_exit()                                  \
    if (CMD_Flag)                                   \
        CMD_Flag = 0;                               \
    if (!strcmp(CMD_Buffer, "exit"))                \
    {                                               \
        printf("exit\r\n");                         \
        break;                                      \
    }                                               \
    for (int i = CMD_Buffer_Count + 2; i >= 0; i--) \
    {                                               \
        CMD_Buffer[i] = 0;                          \
    }                                               \
    CMD_Buffer_Count = 0;

void reboot(void);
void help(void);
void cmd(void);
void debug_on(void);
void debug_off(void);
void show_encoder(void);
void show_foc(void);
void set_uqud(void);
void set_pos(void);
void set_pid(void);
void set_speed(void);

#endif
