#ifndef __BSP_PID_H__
#define __BSP_PID_H__

#include <stdio.h>

#define  P_DATA                   0.5               //P参数
#define  I_DATA                   0.06              //I参数
#define  D_DATA                   0                 //D参数


//定义PID结构体
typedef struct 
{
   int      SetPoint;      //设定目标 Desired Value
   double   Proportion;    //比例常数 Proportional Const
   double   Integral;      //积分常数 Integral Const
   double   Derivative;    //微分常数 Derivative Const
   int      LastError;     //Error[-1]
   int      PrevError;     //Error[-2]
}PID_TypeDef;

extern PID_TypeDef bldc_pid;

void IncPIDInit(void) ;
int IncPIDCalc(int NextPoint) ;
#endif











