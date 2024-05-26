#include "bsp_pid.h"
#include "bsp_motor.h"

PID_TypeDef bldc_pid;

void IncPIDInit(void) 
{
  bldc_pid.LastError=0;                    //Error[-1]
  bldc_pid.PrevError=0;                    //Error[-2]
  bldc_pid.Proportion=P_DATA;              //比例常数 Proportional Const
  bldc_pid.Integral=I_DATA;                //积分常数  Integral Const
  bldc_pid.Derivative=D_DATA;              //微分常数 Derivative Const
  bldc_pid.SetPoint=bldc_dev.motor_speed;  //设定目标Desired Value
}

int IncPIDCalc(int NextPoint) 
{
  int iError,iIncpid;                                       //当前误差
  iError=bldc_pid.SetPoint - NextPoint;                     //增量计算
  iIncpid=(bldc_pid.Proportion * iError)                    //E[k]项
              -(bldc_pid.Integral * bldc_pid.LastError)     //E[k-1]项
              +(bldc_pid.Derivative * bldc_pid.PrevError);  //E[k-2]项
              
  bldc_pid.PrevError=bldc_pid.LastError;                    //存储误差，用于下次计算
  bldc_pid.LastError=iError;

  return(iIncpid);                                    //返回增量值
}





