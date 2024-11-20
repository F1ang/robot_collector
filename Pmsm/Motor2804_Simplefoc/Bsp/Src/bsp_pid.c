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



/* 自平衡小车版PID：因为IMU的存在，利用起来 */

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

    VerPD->output = VerPD->kp*VerPD->err + Gyro*VerPD->kd;   //===计算平衡控制的电机PWM  PD控制   kp是P系数 kd是D系数
    //输出限幅
    if(VerPD->output > VerPD->max_out) VerPD->output = VerPD->max_out;
    else if(VerPD->output < -VerPD->max_out) VerPD->output = -VerPD->max_out;

    //return VerPD->output;
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
    //printf("encoder = %d, %d\r\n", encoder_left, encoder_right);
    //一阶低通滤波器
    VerPI->err *= 0.8;
    VerPI->err += VerPI->old_err * 0.2;
    VerPI->old_err = VerPI->err;
    //printf("err = %.2f\r\n", VerPI->err);
    //闭环死区
    //if((fabs(VerPI->err) > VerPI->death) && (VerPI->is_death == 1))
    //{
    //	return 0;
    //}

    //误差积累
    VerPI->i_out += VerPI->err;

    //printf("i_out = %.2f\r\n", VerPI->i_out);

    //积分限幅
    if(VerPI->i_out > VerPI->max_err_integral) VerPI->i_out = VerPI->max_err_integral;
    else if(VerPI->i_out < -VerPI->max_err_integral) VerPI->i_out = -VerPI->max_err_integral;

    VerPI->output = VerPI->kp * VerPI->err + VerPI->ki * VerPI->i_out;

    //输出限幅
    if(VerPI->output > VerPI->max_out) VerPI->output = VerPI->max_out;
    else if(VerPI->output < -VerPI->max_out) VerPI->output = -VerPI->max_out;

    //return VerPI->output;
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


//float Med_Angle=-12;	//机械中值。---在这里修改你的机械中值即可。
//float Target_Speed=0;	//期望速度（俯仰）。---二次开发接口，用于控制小车前进后退及其速度。

// float 
// Vertical_Kp=-390,//420*0.6=252     770*0.6=462   -1300*0.6=-780    -870  -1050*0.6
// Vertical_Kd=0.01;    //1*0.6     2.3*0.6=1.38  -3*0.6=-1.8   -4.2*0.6  -3*0.6

// float 
// Velocity_Kp=0.45,//速度环KP、KI  0.21   0.45
// Velocity_Ki=0.005;      ////   0.0015   0.004

#define SPEED_KP 0.22  // 0.22 0.075  0.035  

float  Vertical_Kp = -370; //   -370  -660   -550  -570*0.6=      -330  -336 770*0.6=462   -1300*0.6=-780    -870  -1050*0.6
float  Vertical_Kd = -25; //    -30/-25   -8  -0.2*0.6

float Velocity_Kp = -SPEED_KP;              //速度环KP、KI  0.21   0.45
float Velocity_Ki = -SPEED_KP/200;      ////   0.0015   0.004

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
	static int Encoder_S,EnC_Err_Lowout_last,PWM_out,Encoder_Err,EnC_Err_Lowout;
	float a=0.7;
	
	//1.计算速度偏差
	Encoder_Err=((encoder_left+encoder_right)-Target);//舍去误差--我的理解：能够让速度为"0"的角度，就是机械中值。
	//2.对速度偏差进行低通滤波
	//low_out=(1-a)*Ek+a*low_out_last;
	EnC_Err_Lowout=(1-a)*Encoder_Err+a*EnC_Err_Lowout_last;//使得波形更加平滑，滤除高频干扰，防止速度突变。
	EnC_Err_Lowout_last=EnC_Err_Lowout;//防止速度过大的影响直立环的正常工作。
	//3.对速度偏差积分，积分出位移
	Encoder_S+=EnC_Err_Lowout;
	//4.积分限幅
	Encoder_S=Encoder_S>10000?10000:(Encoder_S<(-10000)?(-10000):Encoder_S);
	
	//5.速度环控制输出计算
	PWM_out=Velocity_Kp*EnC_Err_Lowout+Velocity_Ki*Encoder_S;

	PWM_out = PWM_out > 10000 ? 10000:PWM_out;
	
	return PWM_out;
}



//*****************************************************************
//函数功能：控制小车速度
//encoder_left： 左轮编码器值
//encoder_right：右轮编码器值   因为程序周期执行，所以这里编码器的值可以理解为速度
int sudu(int Target, int encoder_left,int encoder_right)
{  
	  static int pwm_sudu,Encoder_Least,Encoder;
	  static int Encoder_Integral;	
		Encoder_Least =(encoder_left+encoder_right)-Target;  //获取最新速度偏差==测量速度（左右编码器之和）-目标速度（此次为零） 
		Encoder *= 0.8;		                             //一阶低通滤波器       
		Encoder += Encoder_Least*0.2;	                 //一阶低通滤波器    
  	Encoder_Integral +=Encoder;                     //积分出位移 积分时间：5ms
		if(Encoder_Integral>8000)  Encoder_Integral=8000;  //积分限幅
		if(Encoder_Integral<-8000)	Encoder_Integral=-8000; //积分限幅	
	
		pwm_sudu=Velocity_Kp*Encoder+Velocity_Ki*Encoder_Integral;     //速度PI控制器	
//		if((pitch>=80)||(pitch<=-80))  //小车跌倒后清零
//		{
//		  Encoder_Integral=0;    
//		}			
	  return pwm_sudu;
}
