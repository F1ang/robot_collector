/*
	1、RM官方版
	2、电机控制版
	3、HLL步兵版
*/

//error->闭环死区(允许误差范围)
//error->积分分离阈值 ->积分分离->防止积分饱和----本质:静差存在,error积累很大
//积分不分离，则进行积分(项)限幅
//本质:偏差的小不看，积分的大不要
	
//1、RM官方版
//位置式和增量式PID，积分限幅，输出限幅，历程计，实现
#include "pid_hll.h"
/*限幅*/
#define  LimitMax(input, max)   \
    {                          \
        if (input > max)       \
        {                      \
            input = max;       \
        }                      \
        else if (input < -max) \
        {                      \
            input = -max;      \
        }                      \
    }
/*init*/   
void PID_Init(PidTypeDef *pid, uint8_t mode, const float PID[3], float max_out, float max_iout)
{
    if (pid == NULL || PID == NULL)
    {
        return;
    }
    pid->mode = mode;
    pid->Kp = PID[0]; 
    pid->Ki = PID[1];
    pid->Kd = PID[2];
    pid->max_out = max_out;
    pid->max_iout = max_iout;
    pid->Dbuf[0] = pid->Dbuf[1] = pid->Dbuf[2] = 0.0f;
    pid->error[0] = pid->error[1] = pid->error[2] = pid->Pout = pid->Iout = pid->Dout = pid->out = 0.0f;
}
/*位置和增量PID*/
float PID_Calc(PidTypeDef *pid, float ref, float set)
{		
    if (pid == NULL)
    {
        return 0.0f;
    }
    pid->error[2] = pid->error[1];
    pid->error[1] = pid->error[0];
    pid->set = set;
    pid->fdb = ref;
    pid->error[0] = set - ref;
    if (pid->mode == PID_POSITION)     
    {
        pid->Pout = pid->Kp * pid->error[0];
        pid->Iout += pid->Ki * pid->error[0];
        pid->Dbuf[0] = (pid->error[0] - pid->error[1]);
        pid->Dout = pid->Kd * pid->Dbuf[0];
			
        LimitMax(pid->Iout, pid->max_iout);//积分限幅		
        pid->out = pid->Pout + pid->Iout + pid->Dout;
        LimitMax(pid->out, pid->max_out);//输出限幅
    }
    else if (pid->mode == PID_DELTA)
    {
        pid->Pout = pid->Kp * (pid->error[0] - pid->error[1]);
        pid->Iout = pid->Ki * pid->error[0];
        pid->Dbuf[2] = pid->Dbuf[1];
        pid->Dbuf[1] = pid->Dbuf[0];
        pid->Dbuf[0] = (pid->error[0] - 2.0f * pid->error[1] + pid->error[2]);
        pid->Dout = pid->Kd * pid->Dbuf[0];
        pid->out += pid->Pout + pid->Iout + pid->Dout;
			
        LimitMax(pid->out, pid->max_out);
    }
    return pid->out;
}
/*reset*/
void PID_clear(PidTypeDef *pid)
{
    if (pid == NULL)
    {
        return;
    }
    pid->error[0] = pid->error[1] = pid->error[2] = 0.0f;
    pid->Dbuf[0] = pid->Dbuf[1] = pid->Dbuf[2] = 0.0f;
    pid->out = pid->Pout = pid->Iout = pid->Dout = 0.0f;
    pid->fdb = pid->set = 0.0f;
}
/*Milemeter*/
void set_pid_target(Milemeter *pid,float temp_val)
{  
	pid->target_val = temp_val;    // 设置当前的目标值
	return ;
}




//2、电机控制版
//位置环PID，速度环PID，闭环死区(误差范围)，积分分离(积分项-静差存在)，输出限幅，实现
/*
#include "bsp_pid.h"
#include "capture.h"
#include "protocol.h"
#include "common.h"*/
/*定义位置PID与速度PID结构体型的全局变量*/
PID pid_location;
PID pid_speed;
/**
  * @brief  PID参数初始化
  *	@note 	无
  * @retval 无
  */
void PID_param_init()
{
	/* 位置相关初始化参数 */
	pid_location.target_val = TOTAL_RESOLUTION*10;				
	pid_location.output_val = 0.0;
	pid_location.err = 0.0;
	pid_location.err_last = 0.0;
	pid_location.integral = 0.0;

	pid_location.Kp = 0.05;
	pid_location.Ki = 0;
	pid_location.Kd = 0;

	/* 速度相关初始化参数 */
	pid_speed.target_val=10.0;				
	pid_speed.output_val=0.0;
	pid_speed.err=0.0;
	pid_speed.err_last=0.0;
	pid_speed.integral=0.0;

	pid_speed.Kp = 80.0;
	pid_speed.Ki = 2.0;
	pid_speed.Kd = 100.0;
}

extern float det_plus;
/**
  * @brief  设置目标值
  * @param  val		目标值
  *	@note 	无
  * @retval 无
  */
void set_pid_target(PID *pid, float temp_val)
{  
	pid->target_val = temp_val;    // 设置当前的目标值
}

/**
  * @brief  获取目标值
  * @param  无
  *	@note 	无
  * @retval 目标值
  */
float get_pid_target(PID *pid)
{
  return pid->target_val;    // 获取当前的目标值
}

/**
  * @brief  设置比例、积分、微分系数
  * @param  p：比例系数 P
  * @param  i：积分系数 i
  * @param  d：微分系数 d
  *	@note 	无
  * @retval 无
  */
void set_p_i_d(PID *pid, float p, float i, float d)
{
	pid->Kp = p;    // 设置比例系数 P
	pid->Ki = i;    // 设置积分系数 I
	pid->Kd = d;    // 设置微分系数 D
}

//目标速度值限制（位置环输出）
static float TARGET_SPEED_MAX = 60.0;
static void speed_val_protect(float *speed_val)
{
	/*目标速度上限处理*/
	if (*speed_val > TARGET_SPEED_MAX)
	{
		*speed_val = TARGET_SPEED_MAX;
	}
	else if (*speed_val < -TARGET_SPEED_MAX)
	{
		*speed_val = -TARGET_SPEED_MAX;
	}	
}
//PWM值限制（速度环输出）
static int pwm_val_protect(int pwm_input)
{
	int pwm_output = 0;	
    //限幅
	if(pwm_input>999) 
	{
		pwm_output = 999;
	}
	else if(pwm_input<-999) 
	{
		pwm_output = -999;
	}
	else if((pwm_input>-50)&&(pwm_input<50)) 
	{
		pwm_output = 0;
	}
    //正常
	else
	{
		pwm_output = pwm_input;
	}	

	return pwm_output;
}

/**
  * @brief  位置PID算法实现
  * @param  actual_val:实际值
  *	@note 	无
  * @retval 通过PID计算后的输出
  */
#define LOC_DEAD_ZONE 60 /*位置环死区*/
#define LOC_INTEGRAL_START_ERR 200 /*积分分离时对应的误差范围*/
#define LOC_INTEGRAL_MAX_VAL 800   /*积分范围限定，防止积分饱和*/
float location_pid_realize(PID *pid, float actual_val)
{
	/*计算目标值与实际值的误差*/
	pid->err = pid->target_val - actual_val;

	/* 设定闭环死区:偏差太小 */
	if((pid->err >= -LOC_DEAD_ZONE) && (pid->err <= LOC_DEAD_ZONE))
	{
		pid->err = 0;
		pid->integral = 0;
		pid->err_last = 0;
	}

	/*积分项：积分分离，偏差较大时去掉积分作用:偏差太大*/
	if(pid->err > -LOC_INTEGRAL_START_ERR && pid->err < LOC_INTEGRAL_START_ERR)
	{
		pid->integral += pid->err;  
        /*积分项限幅，防止积分饱和*/
		if(pid->integral > LOC_INTEGRAL_MAX_VAL)
		{
			pid->integral = LOC_INTEGRAL_MAX_VAL;
		}
		else if(pid->integral < -LOC_INTEGRAL_MAX_VAL)
		{
			pid->integral = -LOC_INTEGRAL_MAX_VAL;
		}
	}	

	/*PID算法实现*/
	pid->output_val = pid->Kp * pid->err +
	                  pid->Ki * pid->integral +
	                  pid->Kd * (pid->err - pid->err_last);

	/*误差传递*/
	pid->err_last = pid->err;

	return pid->output_val;
}

/**
  * @brief  速度PID算法实现
  * @param  actual_val:实际值
  *	@note 	无
  * @retval 通过PID计算后的输出
  */
#define SPE_DEAD_ZONE 5.0f /*速度环死区*/
#define SPE_INTEGRAL_START_ERR 100 /*积分分离时对应的误差范围*/
#define SPE_INTEGRAL_MAX_VAL 260   /*积分范围限定，防止积分饱和*/
float speed_pid_realize(PID *pid, float actual_val)
{
	pid->err = pid->target_val - actual_val;

	/* 设定闭环死区 */
	if( (pid->err>-SPE_DEAD_ZONE) && (pid->err<SPE_DEAD_ZONE ) )
	{
		pid->err = 0;
		pid->integral = 0;
		pid->err_last = 0;
	}

#if 0
	/*积分项*/
	pid->integral += pid->err;
#else	
	/*积分项，积分分离，偏差较大时去掉积分作用*/
	if(pid->err > -SPE_INTEGRAL_START_ERR && pid->err < SPE_INTEGRAL_START_ERR)
	{
		pid->integral += pid->err;  
        /*积分范围限定，防止积分饱和*/
		if(pid->integral > SPE_INTEGRAL_MAX_VAL)
		{
			pid->integral = SPE_INTEGRAL_MAX_VAL;
		}
		else if(pid->integral < -SPE_INTEGRAL_MAX_VAL)
		{
			pid->integral = -SPE_INTEGRAL_MAX_VAL;
		}
	}	
#endif

	pid->output_val = pid->Kp * pid->err +
	                  pid->Ki * pid->integral +
	                  pid->Kd *(pid->err - pid->err_last);
	pid->err_last = pid->err;

	return pid->output_val;
}




//3、HLL步兵版
//位置环PID，速度环PID，闭环死区，积分分离，过零处理，角度PID，
/*#include "pid.h"
#include "math.h"*/
static float Pid_Limit(float value, float min, float max)
{
	if(value < min)
		return min;
	if(value > max)
		return max;
	
	return value;
}
/*过零处理*/
static float CorrectingZeroDirection(float err)
{
	if(err>4095){return err-8191;}
	else if(err<-4095){return err+8191;}
	else {return err;}
}

/*位置式PID*/
static void Calc_Position_Pid(Pid_Position_t* pid)
{
	float intergal_spare_k = 1.0f;
	
	pid->err = pid->tar - pid->cur;
	/*积分分离*/
	if(pid->is_integral_spare)
	{
		if(fabs(pid->err) > pid->begin_integral)//大不要
		{
			intergal_spare_k = 0.0f;
		}
		else if(fabs(pid->err) < pid->stop_grow_integral)//小不看
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

//增量式PID
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



//位置PID
float Pid_Position_Calc(Pid_Position_t* pid, float tar, float cur)
{
	pid->tar = tar;
	pid->cur = cur;
	Calc_Position_Pid(pid);
	return pid->output;
}
//增量PID
float Pid_Increment_Calc(Pid_Increment_t* pid, float tar, float cur)
{
	pid->tar = tar;
	pid->cur = cur;
	Calc_Increment_Pid(pid);
	return pid->output;
}


//注：下面暂时用不到
//角度PID
static void Calc_Position_Angle_Pid(Pid_Position_t* pid)
{
	//float intergal_spare_k = 1.0f;
	
	pid->err = pid->tar - pid->cur;

	//pid->err=CorrectingZeroDirection(pid->err);//零环回正
	/*闭环死区*/
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
	pid->i_out = pid->ki * pid->err_integral;
	pid->d_out = pid->kd * (pid->err - pid->old_err);
	
	pid->output = pid->p_out + pid->i_out + pid->d_out;
	
	/*输出限幅*/
	pid->output = Pid_Limit(pid->output, -pid->max_out, pid->max_out);
	
	pid->old_err = pid->err;
}
//角度PID
float Pid_Position_Angle_Calc(Pid_Position_t* pid, float tar, float cur)
{
	pid->tar = tar;
	pid->cur = cur;
	Calc_Position_Angle_Pid(pid);
	return pid->output;
}

闭环死区：闭环死区是指执行机构的最小控制量，无法再通过调节来满足控制精度，如果仍然持续调节，系统则会在目标值前后频繁动作，不能稳定下来。
如某个系统的控制精度是1，但目标值需要是1.5，则无论怎么调节，最终的结果只能控制在 1或 2，始终无法达到预设值。
这 1.5L小数点后的范围，就是闭环死区，系统是无法控制的，误差会一直存在，容易发生震荡现象。

积分分离：通过积分分离的方式来实现抗积分饱和，积分饱和是指执行机构达到极限输出能力了，仍无法到达目标值，在很长一段时间内无法消除静差造成的。
如，PWM输出到了100%，仍达不到期望位置，此时若一直进行误差累加，在一段时间后， PID 的积分项累计了很大的数值，如果这时候到达了目标值或者重新设定了目标值，由于积分由于累计的误差很大，
系统并不能立即调整到目标值，可能造成超调或失调的现象。等效于只用了PD。



