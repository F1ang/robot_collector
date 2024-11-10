//1、RM官方版
#ifndef __PID_HLL_H
#define	__PID_HLL_H
/*#include "sys.h" 
#include <stdio.h>
#include <stdlib.h>
#include <math.h>*/

enum PID_MODE
{
    PID_POSITION = 0,
    PID_DELTA
};
typedef struct
{
    uint8_t mode;
    //PID 三参数
    float Kp;
    float Ki;
    float Kd;

    float max_out;  //最大输出
    float max_iout; //最大积分输出

    float set;
    float fdb;
    float out;
	
    float Pout;
    float Iout;
    float Dout;
	
    float Dbuf[3];  //微分项 0最新 1上一次 2上上次
    float error[3]; //误差项 0最新 1上一次 2上上次
			
} PidTypeDef;

typedef struct 
{
   		int target_val;   //目标值
		int enc_cnt[2];	  //0当前计数 1上一时刻计数
		int enc_cnt1[2];  //0当前计数 1上一时刻计数
		int enc_delta[2]; //1  2 变化值
}Milemeter;

extern void PID_Init(PidTypeDef *pid, uint8_t mode, const float PID[3], float max_out, float max_iout);
extern float PID_Calc(PidTypeDef *pid, float ref, float set);
extern void PID_clear(PidTypeDef *pid);

extern void set_pid_target(PidTypeDef *pid,float temp_val);   
#endif

//2、电机控制版
#ifndef __BSP_PID_H
#define	__BSP_PID_H
/*#include "stm32f4xx.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>*/

typedef struct
{
	float target_val;   //目标值
	float err;          //偏差值
	float err_last;     //上一个偏差值
	float Kp,Ki,Kd;     //比例、积分、微分系数
	float integral;     //积分值
	float output_val;   //输出值

   // float output_max_val;   //输出值
}PID;

extern PID pid_location;
extern PID pid_speed;

void  PID_param_init(void);
void  set_pid_target(PID *pid, float temp_val);
float get_pid_target(PID *pid);
void  set_p_i_d(PID *pid, float p, float i, float d);
float location_pid_realize(PID *pid, float actual_val);
float speed_pid_realize(PID *pid, float actual_val);

#endif


//3、HLL步兵版
#ifndef __PID_H
#define __PID_H

#include "sys.h"

#define NEW_POSITION_PID(p,i,d,limit_i,limit_out,is_i_spare,begin_i,stop_grow_i) {\
	.kp                     = p,\
	.ki                     = i,\
	.kd                     = d,\
	.tar                    = 0,\
	.cur                    = 0,\
	.err                    = 0,\
	.old_err                = 0,\
	.err_integral           = 0,\
	.is_integral_spare      = is_i_spare,\
	.begin_integral         = begin_i,\
	.stop_grow_integral     = stop_grow_i,\
	.p_out                  = 0,\
	.i_out                  = 0,\
	.d_out                  = 0,\
	.output                 = 0,\
	.max_err_integral       = limit_i,\
	.max_out                = limit_out\
}

#define NEW_INCREMENT_PID(p,i,d,limit) {\
	.kp                     = p,\
	.ki                     = i,\
	.kd                     = d,\
	.tar                    = 0,\
	.cur                    = 0,\
	.err                    = 0,\
	.old_err                = 0,\
	.old_old_err            = 0,\
	.p_out                  = 0,\
	.i_out                  = 0,\
	.d_out                  = 0,\
	.output                 = 0,\
	.max_out                = limit\
}

typedef struct
{
	float kp;
	float ki;
	float kd;
	
	float tar;
	float cur;
	
	float err;          //误差值
	float old_err;      //上次误差值
	float err_integral; //积分值
	
	u8    is_integral_spare;      //选择是否使用变积分分离
	//此时的积分系数为ki*index
	float begin_integral;         //当误差大于begin_integral时，不积分
	float stop_grow_integral;     //当误差小于stop_grow_integral时，积分系数处于最大值
	
	float max_err_integral;//积分限幅----max_out/20
	float max_out;  //最大输出
	
	float p_out;
	float i_out;
	float d_out;
	float output;
	
}Pid_Position_t;

typedef struct
{
	//PID 三参数
	float kp;
	float ki;
	float kd;
	
	float tar;
	float cur;
	
	float err;
	float old_err;
	float old_old_err;
	
	float max_out;  //最大输出
	
	float p_out;
	float i_out;
	float d_out;
	
	float output;
	
}Pid_Increment_t;

float Pid_Position_Calc(Pid_Position_t* pid, float tar, float cur);
float Pid_Increment_Calc(Pid_Increment_t* pid, float tar, float cur);
float Pid_Position_Angle_Calc(Pid_Position_t* pid, float tar, float cur);
#endif
