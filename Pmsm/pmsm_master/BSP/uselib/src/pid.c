#include "pid.h"

PID pid_location;
PID pid_speed;

#define LOC_DEAD_ZONE 60 /*位置环死区--误差*/
#define LOC_INTEGRAL_START_ERR 200 /*积分分离--误差*/
#define LOC_INTEGRAL_MAX_VAL 800   /*积分饱和--积分项*/

#define SPE_DEAD_ZONE 5.0f 					/*速度环死区*/
#define SPE_INTEGRAL_START_ERR 100 	/*积分分离*/
#define SPE_INTEGRAL_MAX_VAL 260   	/*积分饱和*/

static float TARGET_SPEED_MAX = 60.0;
static float TARGET_PWM_MAX =6000;
void PID_param_init(void)
{
	/* 位置相关初始化参数 */
	pid_location.target_val = 0;				
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


//设置目标值
void set_pid_target(PID *pid, float temp_val)
{  
	pid->target_val = temp_val;    
}

//获取目标值
float get_pid_target(PID *pid)
{
  return pid->target_val;   
}

//设置比例、积分、微分系数
void set_p_i_d(PID *pid, float p, float i, float d)
{
	pid->Kp = p;    
	pid->Ki = i;    
	pid->Kd = d;    
}


//位置环输出
void speed_val_protect(float *speed_val)
{
	/*限幅*/
	if (*speed_val > TARGET_SPEED_MAX)
	{
		*speed_val = TARGET_SPEED_MAX;
	}
	else if (*speed_val < -TARGET_SPEED_MAX)
	{
		*speed_val = -TARGET_SPEED_MAX;
	}	
}
//速度环输出（PWM值限制）
int pwm_val_protect(int pwm_input)
{
	int pwm_output = 0;	
    //限幅
	if(pwm_input>TARGET_PWM_MAX) 
	{
		pwm_output = TARGET_PWM_MAX;
	}
	else if(pwm_input<-TARGET_PWM_MAX) 
	{
		pwm_output = -TARGET_PWM_MAX;
	}
	else if((pwm_input>-TARGET_PWM_MAX)&&(pwm_input<TARGET_PWM_MAX)) 
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


//位置PID算法实现
float location_pid_realize(PID *pid, float actual_val)
{
	pid->err = pid->target_val - actual_val;
	
	/* 闭环死区 */
	if((pid->err >= -LOC_DEAD_ZONE) && (pid->err <= LOC_DEAD_ZONE))
	{
		pid->err = 0;
		pid->integral = 0;
		pid->err_last = 0;
	}

	/*积分分离->积分项，偏差较大时去掉积分作用*/
	if(pid->err > -LOC_INTEGRAL_START_ERR && pid->err < LOC_INTEGRAL_START_ERR)
	{
		pid->integral += pid->err;  
     /*防止积分饱和*/
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

//速度PID算法实现
float speed_pid_realize(PID *pid, float actual_val)
{
	pid->err = pid->target_val - actual_val;

	/* 闭环死区 */
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







