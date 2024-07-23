#include "pid.h"

PID pid_location;
PID pid_speed;

#define LOC_DEAD_ZONE 60 /*λ�û�����--���*/
#define LOC_INTEGRAL_START_ERR 200 /*���ַ���--���*/
#define LOC_INTEGRAL_MAX_VAL 800   /*���ֱ���--������*/

#define SPE_DEAD_ZONE 5.0f 					/*�ٶȻ�����*/
#define SPE_INTEGRAL_START_ERR 100 	/*���ַ���*/
#define SPE_INTEGRAL_MAX_VAL 260   	/*���ֱ���*/

static float TARGET_SPEED_MAX = 60.0;
static float TARGET_PWM_MAX =6000;
void PID_param_init(void)
{
	/* λ����س�ʼ������ */
	pid_location.target_val = 0;				
	pid_location.output_val = 0.0;
	pid_location.err = 0.0;
	pid_location.err_last = 0.0;
	pid_location.integral = 0.0;

	pid_location.Kp = 0.05;
	pid_location.Ki = 0;
	pid_location.Kd = 0;

	/* �ٶ���س�ʼ������ */
	pid_speed.target_val=10.0;				
	pid_speed.output_val=0.0;
	pid_speed.err=0.0;
	pid_speed.err_last=0.0;
	pid_speed.integral=0.0;

	pid_speed.Kp = 80.0;
	pid_speed.Ki = 2.0;
	pid_speed.Kd = 100.0;
}


//����Ŀ��ֵ
void set_pid_target(PID *pid, float temp_val)
{  
	pid->target_val = temp_val;    
}

//��ȡĿ��ֵ
float get_pid_target(PID *pid)
{
  return pid->target_val;   
}

//���ñ��������֡�΢��ϵ��
void set_p_i_d(PID *pid, float p, float i, float d)
{
	pid->Kp = p;    
	pid->Ki = i;    
	pid->Kd = d;    
}


//λ�û����
void speed_val_protect(float *speed_val)
{
	/*�޷�*/
	if (*speed_val > TARGET_SPEED_MAX)
	{
		*speed_val = TARGET_SPEED_MAX;
	}
	else if (*speed_val < -TARGET_SPEED_MAX)
	{
		*speed_val = -TARGET_SPEED_MAX;
	}	
}
//�ٶȻ������PWMֵ���ƣ�
int pwm_val_protect(int pwm_input)
{
	int pwm_output = 0;	
    //�޷�
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
    //����
	else
	{
		pwm_output = pwm_input;
	}	

	return pwm_output;
}


//λ��PID�㷨ʵ��
float location_pid_realize(PID *pid, float actual_val)
{
	pid->err = pid->target_val - actual_val;
	
	/* �ջ����� */
	if((pid->err >= -LOC_DEAD_ZONE) && (pid->err <= LOC_DEAD_ZONE))
	{
		pid->err = 0;
		pid->integral = 0;
		pid->err_last = 0;
	}

	/*���ַ���->�����ƫ��ϴ�ʱȥ����������*/
	if(pid->err > -LOC_INTEGRAL_START_ERR && pid->err < LOC_INTEGRAL_START_ERR)
	{
		pid->integral += pid->err;  
     /*��ֹ���ֱ���*/
		if(pid->integral > LOC_INTEGRAL_MAX_VAL)
		{
			pid->integral = LOC_INTEGRAL_MAX_VAL;
		}
		else if(pid->integral < -LOC_INTEGRAL_MAX_VAL)
		{
			pid->integral = -LOC_INTEGRAL_MAX_VAL;
		}
	}	

	/*PID�㷨ʵ��*/
	pid->output_val = pid->Kp * pid->err +
	                  pid->Ki * pid->integral +
	                  pid->Kd * (pid->err - pid->err_last);

	/*����*/
	pid->err_last = pid->err;

	return pid->output_val;
}

//�ٶ�PID�㷨ʵ��
float speed_pid_realize(PID *pid, float actual_val)
{
	pid->err = pid->target_val - actual_val;

	/* �ջ����� */
	if( (pid->err>-SPE_DEAD_ZONE) && (pid->err<SPE_DEAD_ZONE ) )
	{
		pid->err = 0;
		pid->integral = 0;
		pid->err_last = 0;
	}

#if 0
	/*������*/
	pid->integral += pid->err;
#else	
	/*��������ַ��룬ƫ��ϴ�ʱȥ����������*/
	if(pid->err > -SPE_INTEGRAL_START_ERR && pid->err < SPE_INTEGRAL_START_ERR)
	{
		pid->integral += pid->err;  
        /*���ַ�Χ�޶�����ֹ���ֱ���*/
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







