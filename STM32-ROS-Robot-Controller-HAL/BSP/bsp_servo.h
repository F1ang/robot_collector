#ifndef BSP_SERVO_H
#define BSP_SERVO_H
#include "bsp_define.h"

#define MOTOR_SERVO1(angle)   __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1,angle);
#define MOTOR_SERVO2(angle)   __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2,angle);
#define MOTOR_SERVO3(angle)   __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3,angle);

typedef enum
{
	SERVO1 = 1u,
	SERVO2,
	SERVO3,
}id;

void ServoInit(void);
void ServoCtrl(unsigned int id,float num);

#endif

..\OBJ\LVGL.axf: Error: L6218E: Undefined symbol ILI9341_DrawPixel (referred from lv_port_disp_template.o).


disp=>初始化+打点函数+缓冲


..\OBJ\LVGL.axf: Error: L6218E: Undefined symbol img_clothes (referred from lv_demo_widgets.o).