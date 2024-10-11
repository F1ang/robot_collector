#include "bsp_kalaman.h"

/***********************宏定义***********************/
#define Kp 40.0f      //Kp比例增益 决定了加速度计和磁力计的收敛速度
	 
#define Ki 0.02f      //Ki积分增益 决定了陀螺仪偏差的收敛速度
		
#define halfT 0.005f  //halfT采样周期的一半
#define dt 0.01f		
/***************************************************/

static float q0=1.0f,q1=0.0f,q2=0.0f,q3=0.0f;
static float exInt = 0, eyInt = 0, ezInt = 0; 
static float newdata=0.0f,olddata=0.0f;
float pitchoffset,rolloffset,yawoffset;

static float k10=0.0f,k11=0.0f,k12=0.0f,k13=0.0f;
static float k20=0.0f,k21=0.0f,k22=0.0f,k23=0.0f;
static float k30=0.0f,k31=0.0f,k32=0.0f,k33=0.0f;
static float k40=0.0f,k41=0.0f,k42=0.0f,k43=0.0f;

float invSqrt(float number);
void AHRSupdate(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz,float *roll,float *pitch,float *yaw);
void CalibrateToZero(void);

/*
  AHRS:航姿参考系统
  互补滤波算法融合姿态:
  (1)加速度->俯仰、横滚 磁力计->偏航 陀螺仪->俯仰、横滚、偏航->误差
  (2)地球坐标系与物体坐标系
  微分方程->一阶龙格库塔法
*/

// 快速获得开方的倒数
float invSqrt(float number)
{
	long i;
	float x,y;
	const float f=1.5f;
	
	x=number*0.5f;
	y=number;
	i=*((long*)&y);
	i=0x5f375a86-(i>>1);
	y=*((float *)&i);
	y=y*(f-(x*y*y));
	return y;
}

// 零漂值计算
void CalibrateToZero(void)
{
			uint8_t t=0;
			float sumpitch=0,sumroll=0,sumyaw=0;
			float pitch,roll,yaw;
			short igx,igy,igz;
			short iax,iay,iaz;
			short imx,imy,imz;
			float gx,gy,gz;
			float ax,ay,az;
			float mx,my,mz;
			for (t=0;t<150;t++)
			{
        MPU_Get_Gyro(&igx,&igy,&igz,&gx,&gy,&gz);
        MPU_Get_Accel(&iax,&iay,&iaz,&ax,&ay,&az);
        MPU_Get_Mag(&imx,&imy,&imz,&mx,&my,&mz);
        AHRSupdate(gx,gy,gz,ax,ay,az,mx,my,mz,&roll,&pitch,&yaw);				
        delay_us(6430);
				if (t>=100)
				{
					sumpitch+=pitch;
					sumroll+=roll;
					sumyaw+=yaw;
				}
			}
			pitchoffset=-sumpitch/50.0f;
			rolloffset=-sumroll/50.0f;
			yawoffset=-sumyaw/50.0f;
}

/*******************************************************************************
*函数原型:	 	void AHRS_AHRSupdate
*功　　能:	 	更新AHRS 更新四元数 
*输入参数： 	当前经过校正的测量值
*输入参数：		gx gy gz 三轴角速度 单位rad/s
				ax ay az 三轴加速度（在静态是为重力加速度的三轴投影，无单位，只需要比例关系）当需要加速度积分速度位移时需要单位
				mx my mz 三轴磁场强度 无单位 因为只需要比例关系 
*******************************************************************************/
void AHRSupdate(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz,float *roll,float *pitch,float *yaw)
{
  float norm;									//用于单位化
  float hx, hy, hz, bx, bz;		//
  float vx, vy, vz, wx, wy, wz; 
  float ex, ey, ez;

  // 辅助变量减少重复操作次数
  float q0q0 = q0*q0;
  float q0q1 = q0*q1;
  float q0q2 = q0*q2;
  float q0q3 = q0*q3;
  float q1q1 = q1*q1;
  float q1q2 = q1*q2;
  float q1q3 = q1*q3;
  float q2q2 = q2*q2;
  float q2q3 = q2*q3;
  float q3q3 = q3*q3;

  // 对加速度计和磁力计数据进行单位化(外积使用)
  norm = invSqrt(ax*ax + ay*ay + az*az);
  ax = ax * norm;
  ay = ay * norm;
  az = az * norm;
  norm = invSqrt(mx*mx + my*my + mz*mz);
  mx = mx * norm;
  my = my * norm;
  mz = mz * norm;

  // 理论地磁向量及机体参考系
  //hx,hy,hz是mx,my,mz在参考坐标系的表示
  hx = 2*mx*(0.50 - q2q2 - q3q3) + 2*my*(q1q2 - q0q3) + 2*mz*(q1q3 + q0q2);
  hy = 2*mx*(q1q2 + q0q3) + 2*my*(0.50 - q1q1 - q3q3) + 2*mz*(q2q3 - q0q1);
  hz = 2*mx*(q1q3 - q0q2) + 2*my*(q2q3 + q0q1) + 2*mz*(0.50 - q1q1 -q2q2);    
  //bx,by,bz是地球磁场在参考坐标系的表示
  bx = sqrt((hx*hx) + (hy*hy)); // x y分量合并(水平+竖直方向)
  bz = hz;

  // vx,vy,vz是重力加速度在物体坐标系的表示
  vx = 2*(q1q3 - q0q2);
  vy = 2*(q0q1 + q2q3);
  vz = q0q0 - q1q1 - q2q2 + q3q3;

  // wx,wy,wz是地磁场在物体坐标系的表示
  wx = 2*bx*(0.5 - q2q2 - q3q3) + 2*bz*(q1q3 - q0q2);
  wy = 2*bx*(q1q2 - q0q3) + 2*bz*(q0q1 + q2q3);
  wz = 2*bx*(q0q2 + q1q3) + 2*bz*(0.5 - q1q1 - q2q2); 

  // ex,ey,ez是加速度计与磁力计测量出的方向与实际重力加速度与地磁场方向的误差，误差用叉积来表示，且加速度计与磁力计的权重是一样的
  ex = (ay*vz - az*vy) + (my*wz - mz*wy);
  ey = (az*vx - ax*vz) + (mz*wx - mx*wz);
  ez = (ax*vy - ay*vx) + (mx*wy - my*wx);

  // 积分误差
  exInt = exInt + ex*Ki*dt;
  eyInt = eyInt + ey*Ki*dt;
  ezInt = ezInt + ez*Ki*dt;

  // PI调节陀螺仪数据
  gx = gx + Kp*ex + exInt;
  gy = gy + Kp*ey + eyInt;
  gz = gz + Kp*ez + ezInt;

  //欧拉法解微分方程
  //           tmp0 = q0 + (-q1*gx - q2*gy - q3*gz)*halfT;
  //           tmp1 = q1 + ( q0*gx + q2*gz - q3*gy)*halfT;
  //           tmp2 = q2 + ( q0*gy - q1*gz + q3*gx)*halfT;
  //           tmp3 = q3 + ( q0*gz + q1*gy - q2*gx)*halfT; 
  //					 q0=tmp0;
  //					 q1=tmp1;
  //					 q2=tmp2;
  //					 q3=tmp3;
  //printf("q0=%0.1f q1=%0.1f q2=%0.1f q3=%0.1f",q0,q1,q2,q3);
  // RUNGE_KUTTA 法解微分方程(一阶龙格库塔runge-kutta)
  k10=0.5 * (-gx*q1 - gy*q2 - gz*q3);
  k11=0.5 * ( gx*q0 + gz*q2 - gy*q3);
  k12=0.5 * ( gy*q0 - gz*q1 + gx*q3);
  k13=0.5 * ( gz*q0 + gy*q1 - gx*q2);
  
  k20=0.5 * (halfT*(q0+halfT*k10) + (halfT-gx)*(q1+halfT*k11) + (halfT-gy)*(q2+halfT*k12) + (halfT-gz)*(q3+halfT*k13));
  k21=0.5 * ((halfT+gx)*(q0+halfT*k10) + halfT*(q1+halfT*k11) + (halfT+gz)*(q2+halfT*k12) + (halfT-gy)*(q3+halfT*k13));
  k22=0.5 * ((halfT+gy)*(q0+halfT*k10) + (halfT-gz)*(q1+halfT*k11) + halfT*(q2+halfT*k12) + (halfT+gx)*(q3+halfT*k13));
  k23=0.5 * ((halfT+gz)*(q0+halfT*k10) + (halfT+gy)*(q1+halfT*k11) + (halfT-gx)*(q2+halfT*k12) + halfT*(q3+halfT*k13));
  
  k30=0.5 * (halfT*(q0+halfT*k20) + (halfT-gx)*(q1+halfT*k21) + (halfT-gy)*(q2+halfT*k22) + (halfT-gz)*(q3+halfT*k23));
  k31=0.5 * ((halfT+gx)*(q0+halfT*k20) + halfT*(q1+halfT*k21) + (halfT+gz)*(q2+halfT*k22) + (halfT-gy)*(q3+halfT*k23));
  k32=0.5 * ((halfT+gy)*(q0+halfT*k20) + (halfT-gz)*(q1+halfT*k21) + halfT*(q2+halfT*k22) + (halfT+gx)*(q3+halfT*k23));
  k33=0.5 * ((halfT+gz)*(q0+halfT*k20) + (halfT+gy)*(q1+halfT*k21) + (halfT-gx)*(q2+halfT*k22) + halfT*(q3+halfT*k23));
  
  k40=0.5 * (dt*(q0+dt*k30) + (dt-gx)*(q1+dt*k31) + (dt-gy)*(q2+dt*k32) + (dt-gz)*(q3+dt*k33));
  k41=0.5 * ((dt+gx)*(q0+dt*k30) + dt*(q1+dt*k31) + (dt+gz)*(q2+dt*k32) + (dt-gy)*(q3+dt*k33));
  k42=0.5 * ((dt+gy)*(q0+dt*k30) + (dt-gz)*(q1+dt*k31) + dt*(q2+dt*k32) + (dt+gx)*(q3+dt*k33));
  k43=0.5 * ((dt+gz)*(q0+dt*k30) + (dt+gy)*(q1+dt*k31) + (dt-gx)*(q2+dt*k32) + dt*(q3+dt*k33));	
  
  q0=q0 + dt/6.0 * (k10+2*k20+2*k30+k40);
  q1=q1 + dt/6.0 * (k11+2*k21+2*k31+k41);
  q2=q2 + dt/6.0 * (k12+2*k22+2*k32+k42);
  q3=q3 + dt/6.0 * (k13+2*k23+2*k33+k43);
  
  // normalise quaternion
  norm = invSqrt(q0*q0 + q1*q1 + q2*q2 + q3*q3);
  q0 = q0 * norm;
  q1 = q1 * norm;
  q2 = q2 * norm;
  q3 = q3 * norm;
  
  *pitch = asin(-2 * q1 * q3 + 2 * q0 * q2)* 57.3;	// pitch
  *roll  = atan2(2 * q2 * q3 + 2 * q0 * q1, -2 * q1 * q1 - 2 * q2 * q2 + 1)* 57.3;	// roll
  *yaw   = atan2(2*(q1*q2 + q0*q3),q0*q0+q1*q1-q2*q2-q3*q3) * 57.3;	//yaw
}
