#include "anglespeed_task.h"
#include "bsp_as5600.h"
#include "bsp_foc.h"

uint16_t angle_raw;
uint16_t angle;
void AngleSpeed_Task(void const *argument)
{
   printf("anglespeed_task started\r\n");
   for (;;)
   {
      angle_raw = bsp_as5600GetRawAngle();
      angle = bsp_as5600GetAngle();
      osDelay(100);
   }
}
