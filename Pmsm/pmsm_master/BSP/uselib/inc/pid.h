#ifndef  _PID_H
#define  _PID_H

#include "sys.h" 

#include <stdio.h>
#include <stdlib.h>
#include <math.h>


typedef struct
{
	float target_val;   //Ŀ��ֵ
	float err;          //ƫ��ֵ
	float err_last;     //��һ��ƫ��ֵ
	float Kp,Ki,Kd;     //���������֡�΢��ϵ��
	float integral;     //����ֵ
	float output_val;   //���ֵ

  float output_max_val;   //���ֵ
}PID;

extern PID pid_location;
extern PID pid_speed;

void PID_param_init(void);
void set_pid_target(PID *pid, float temp_val);
float get_pid_target(PID *pid);
void set_p_i_d(PID *pid, float p, float i, float d);

void speed_val_protect(float *speed_val);
int pwm_val_protect(int pwm_input);

float location_pid_realize(PID *pid, float actual_val);
float speed_pid_realize(PID *pid, float actual_val);
#endif


