//
// Created by Hacker on 2024/4/28.
//

#include "bsp_pid.h"

#define LOG_TAG "PID"
#include "bsp_log.h"

static float Pid_Limit(float value, float min, float max)
{
    if(value < min)
        return min;
    if(value > max)
        return max;

    return value;
}

/* 过零处理--角度变化误差 */

/* 位置式PID */
static void Calc_Position_Pid(Pid_Position_t* pid)
{
    float intergal_spare_k = 1.0f;

    pid->err = pid->tar - pid->cur;
    /*变积分(积分分离)*/
    if(pid->is_integral_spare)
    {
        if(fabs(pid->err) > pid->begin_integral)
        {
            intergal_spare_k = 0.0f;
        }
        else if(fabs(pid->err) < pid->stop_grow_integral)
        {
            pid->err_integral += pid->err;
            intergal_spare_k = 1.0f;
        }
        else
        {
            pid->err_integral += pid->err;
            intergal_spare_k = (pid->begin_integral - fabs(pid->err)) / (pid->begin_integral - pid->stop_grow_integral);
        }
    }
    else
    {
        pid->err_integral += pid->err;
    }

    /*积分限幅*/
    pid->err_integral = Pid_Limit(pid->err_integral, -pid->max_err_integral, pid->max_err_integral);

    pid->p_out = pid->kp * pid->err;
    pid->i_out = pid->ki * pid->err_integral * intergal_spare_k;
    pid->d_out = pid->kd * (pid->err - pid->old_err);

    pid->output = pid->p_out + pid->i_out + pid->d_out;

    /*输出限幅*/
    pid->output = Pid_Limit(pid->output, -pid->max_out, pid->max_out);

    pid->old_err = pid->err;
}

//位置式
float Pid_Position_Calc(Pid_Position_t* pid, float tar, float cur)
{
    pid->tar = tar;
    pid->cur = cur;
    Calc_Position_Pid(pid);
    return pid->output;
}

/* 增量式 */
static void Calc_Increment_Pid(Pid_Increment_t* pid)
{
    pid->err = pid->tar - pid->cur;

    pid->p_out = pid->kp * (pid->err - pid->old_err);
    pid->i_out = pid->ki * pid->err;
    pid->d_out = pid->kd * (pid->err - 2 * pid->old_err + pid->old_old_err);

    pid->output = pid->p_out + pid->i_out + pid->d_out;

    pid->old_old_err = pid->old_err;
    pid->old_err     = pid->err;
}

//增量式
float Pid_Increment_Calc(Pid_Increment_t* pid, float tar, float cur)
{
    pid->tar = tar;
    pid->cur = cur;
    Calc_Increment_Pid(pid);
    return pid->output;
}

/**
 * @description: 直立环
 * @return {*}
 * @param {Pid_Position_t} *VerPD PD控制
 * @param {float} Target_Angle 目标角度
 * @param {float} Real_Angle 实际角度
 * @param {float} Gyro 实际X轴陀螺仪
 */
void Vertical_LoopPD(Pid_Position_t *VerPD, float Target_Angle ,float Real_Angle,float Gyro)
{
    VerPD->err = Target_Angle - Real_Angle;   						//角度中值

    //闭环死区
    //if((fabs(VerPD->err) > VerPD->death) && (VerPD->is_death == 1))
    //{
    //	return 0;
    //}

    VerPD->output = VerPD->kp*VerPD->err + Gyro*VerPD->kd; 
    if(VerPD->output > VerPD->max_out) VerPD->output = VerPD->max_out;
    else if(VerPD->output < -VerPD->max_out) VerPD->output = -VerPD->max_out;
}



/**
 * @description: 速度环
 * @return {*}
 * @param {Pid_Position_t} *VerPI PI控制
 * @param {int} encoder_left 左轮速度
 * @param {int} encoder_right 右轮速度
 * @param {int16_t} *pi pi[0]--x轴前进后退(期望速度x轴)
 */
void Speed_LoopPI(Pid_Position_t *VerPI, int encoder_left, int encoder_right, int16_t robo_speed_target)
{
    VerPI->err = (encoder_left + encoder_right) - robo_speed_target; //目标速度（此处为零）- 测量速度（左右编码器之和）

    //一阶低通滤波器
    VerPI->err *= 0.8;
    VerPI->err += VerPI->old_err * 0.2;
    VerPI->old_err = VerPI->err;

    //误差积累
    VerPI->i_out += VerPI->err;

    //积分限幅
    if(VerPI->i_out > VerPI->max_err_integral) VerPI->i_out = VerPI->max_err_integral;
    else if(VerPI->i_out < -VerPI->max_err_integral) VerPI->i_out = -VerPI->max_err_integral;

    VerPI->output = VerPI->kp * VerPI->err + VerPI->ki * VerPI->i_out;

    //输出限幅
    if(VerPI->output > VerPI->max_out) VerPI->output = VerPI->max_out;
    else if(VerPI->output < -VerPI->max_out) VerPI->output = -VerPI->max_out;

}

/**
 * @description: 转向环
 * @return {*}
 * @param {Pid_Position_t} *TurnPD PD控制
 * @param {float} GYROZ	实际YAW轴
 * @param {int16_t} *pi 期望YAW轴
 * @param {float} ACCELZ 实际Z轴加速度
 */
void Turn_Speed(Pid_Position_t *TurnPD, float GYROZ, int16_t *pi, float ACCELZ)
{
    TurnPD->err = pi[2] - GYROZ;
    //一阶低通滤波器
    TurnPD->err *= 0.8;
    TurnPD->err += TurnPD->old_err * 0.2;
    TurnPD->old_err = TurnPD->err;

    TurnPD->output = TurnPD->kp * TurnPD->err + TurnPD->kd*ACCELZ;

    //输出限幅
    if(TurnPD->output > TurnPD->max_out) TurnPD->output = TurnPD->max_out;
    else if(TurnPD->output < -TurnPD->max_out) TurnPD->output = -TurnPD->max_out;

    //return TurnPD->output;
}

#define SPEED_KP 0.22  // 0.22 0.075  0.035  

float  Vertical_Kp = -370; //   -370  -660   -550  -570*0.6=      -330  -336 770*0.6=462   -1300*0.6=-780    -870  -1050*0.6
float  Vertical_Kd = -25; //    -30/-25   -8  -0.2*0.6

float Velocity_Kp = -SPEED_KP;         
float Velocity_Ki = -SPEED_KP/200;      

int Vertical_out,Velocity_out;//直立环&速度 的输出变量

/*********************
直立环PD控制器：Kp*Ek+Kd*Ek_D
入口：期望角度、真实角度、真实角速度
出口：直立环输出
*********************/
int Vertical(float Med,float Angle,float gyro_X)
{
	int PWM_out;
	
	PWM_out=Vertical_Kp*(Angle-Med)+Vertical_Kd*(gyro_X-0);
	return PWM_out;
}


/*********************
速度环PI：Kp*Ek+Ki*Ek_S
*********************/
int Velocity(int Target,int encoder_left,int encoder_right)
{
    static int PWM_out,Encoder_Least,Encoder;
    static int Encoder_Integral;	
    
    Encoder_Least =(encoder_left+encoder_right)-Target;  //获取最新速度偏差==测量速度（左右编码器之和）-目标速度（此次为零） 
    Encoder *= 0.8;		                             //一阶低通滤波器       
    Encoder += Encoder_Least*0.2;	                 //一阶低通滤波器    
    Encoder_Integral +=Encoder;                     //积分出位移 积分时间：5ms
    if(Encoder_Integral>8000)  Encoder_Integral=8000;  //积分限幅
    if(Encoder_Integral<-8000)	Encoder_Integral=-8000; //积分限幅	

	PWM_out = Velocity_Kp*Encoder+Velocity_Ki*Encoder_Integral;

	return PWM_out;
}
