#ifndef _KEY_H
#define _KEY_H
#include "sys.h"


#define KEY1       HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0) 
#define KEY2       HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_13)  


#define KEY1_PRES 	1
#define KEY2_PRES	2

void KEY_Init(void);
u8 KEY_Scan(u8 mode);
extern void TestSelf(void);

#endif
