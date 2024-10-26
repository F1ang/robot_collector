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

// 矩阵相乘
void MatrixMultiply(float *A, int A_rows, int A_cols, 
                    float *B, int B_rows, int B_cols, 
                    float *C) {
    // 检查矩阵A的列数是否等于矩阵B的行数
    if (A_cols != B_rows) {
        printf("错误：矩阵A的列数必须等于矩阵B的行数。\n");
        return;
    }
    
    // 初始化结果矩阵C
    for (int i = 0; i < A_rows; i++) {
        for (int j = 0; j < B_cols; j++) {
            C[i * B_cols + j] = 0;  // 先将结果矩阵C初始化为0
        }
    }

    // 矩阵相乘
    for (int i = 0; i < A_rows; i++) {
        for (int j = 0; j < B_cols; j++) {
            for (int k = 0; k < A_cols; k++) {
                C[i * B_cols + j] += A[i * A_cols + k] * B[k * B_cols + j];
            }
        }
    }
}

// 矩阵转置
void MatrixTranspose(float *A, int rows, int cols, float *T) {
    // 进行转置操作
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            T[j * rows + i] = A[i * cols + j];
        }
    }
}

// 矩阵加法
void MatrixAdd(float *A, float *B, float *C, int rows, int cols) {
    // 进行矩阵相加操作
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            C[i * cols + j] = A[i * cols + j] + B[i * cols + j];
        }
    }
}

// UD分解
void UD(float *A, int n, float *U, float *D) {
    // 初始化U和D
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            U[i * n + j] = 0.0f;
            D[i * n + j] = 0.0f;
        }
        D[i * n + i] = A[i * n + i]; // 设置对角线为A的对角线元素
    }

    // 进行UD分解
    for (int i = 0; i < n; i++) {
        for (int j = i; j < n; j++) {
            U[i * n + j] = A[i * n + j];
            for (int k = 0; k < i; k++) {
                U[i * n + j] -= U[i * n + k] * D[k * n + k]; // D的k,k元素
            }
        }
        for (int j = i + 1; j < n; j++) {
            D[j * n + i] = U[j * n + i] / D[i * n + i];
            for (int k = 0; k < i; k++) {
                U[j * n + i] -= U[j * n + k] * D[k * n + k]; // D的k,k元素
            }
        }
    }
}

//矩阵求逆
void MatrixInverse(float *A, int n, float *inv) {
    // 创建一个2D数组来存放增广矩阵
    float *augmented = (float *)malloc(n * n * 2 * sizeof(float));

    // 初始化增广矩阵 [A | I]
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            augmented[i * (2 * n) + j] = A[i * n + j];
            augmented[i * (2 * n) + (j + n)] = (i == j) ? 1.0 : 0.0; // 单位矩阵
        }
    }

    // 高斯消元法
    for (int i = 0; i < n; i++) {
        // 找到主元
        float maxEl = fabs(augmented[i * (2 * n) + i]);
        int maxRow = i;
        for (int k = i + 1; k < n; k++) {
            if (fabs(augmented[k * (2 * n) + i]) > maxEl) {
                maxEl = fabs(augmented[k * (2 * n) + i]);
                maxRow = k;
            }
        }

        // 交换最大行与当前行
        for (int k = 0; k < 2 * n; k++) {
            float tmp = augmented[maxRow * (2 * n) + k];
            augmented[maxRow * (2 * n) + k] = augmented[i * (2 * n) + k];
            augmented[i * (2 * n) + k] = tmp;
        }

        // 将主元归一化
        float divisor = augmented[i * (2 * n) + i];
        for (int k = 0; k < 2 * n; k++) {
            augmented[i * (2 * n) + k] /= divisor;
        }

        // 消去其他行
        for (int k = 0; k < n; k++) {
            if (k != i) {
                float factor = augmented[k * (2 * n) + i];
                for (int j = 0; j < 2 * n; j++) {
                    augmented[k * (2 * n) + j] -= factor * augmented[i * (2 * n) + j];
                }
            }
        }
    }

    // 提取逆矩阵
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            inv[i * n + j] = augmented[i * (2 * n) + (j + n)];
        }
    }

    // 释放内存
    free(augmented);
}

// 矩阵减法
void MatrixSub(float *A, float *B, float *C, int rows, int cols) {
    // 进行矩阵减法操作
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            C[i * cols + j] = A[i * cols + j] - B[i * cols + j];
        }
    }
}


extern uint32_t Tick_ekf;
float now = 0;
float lastUpdate = 0;
float w1 = 0, w2 = 0, w3 = 0;
float H[42] = {0};
float Ht[42] = {0};
float E[42] = {0};
float F1[36] = {0};

static float R[36] = {
  0.1f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
  0.0f, 0.1f, 0.0f, 0.0f, 0.0f, 0.0f,
  0.0f, 0.0f, 0.1f, 0.0f, 0.0f, 0.0f,
  0.0f, 0.0f, 0.0f, 0.1f, 0.0f, 0.0f,
  0.0f, 0.0f, 0.0f, 0.0f, 0.1f, 0.0f,
  0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.1f
};

float X[36] = {0};
float U1[36] = {0};
float D1[36] = {0};
float U1t[36] = {0};
float X1[36] = {0};
float X2[36] = {0};
float K[42] = {0};

float F[49] = {0};
float Ft[49] = {0};
float P[49] = {0};
float A[49] = {0};
static float B[49] = {0};
float Q[49] = {
  0.1f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
  0.0f, 0.1f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
  0.0f, 0.0f, 0.1f, 0.0f, 0.0f, 0.0f, 0.0f,
  0.0f, 0.0f, 0.0f, 0.1f, 0.0f, 0.0f, 0.0f,
  0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.1f, 0.0f,
  0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.1f,
  0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.1f
};

float P1[49] = {0};

float T[6] = {0};
float Y[7] = {0};

float Z[42] = {0};
float I[42] = {0};
float O[42] = {0};
/**************************实现函数********************************************
*函数原型:	 	void AHRS_AHRSupdate
*功　　能:	 	更新AHRS 更新四元数 
*输入参数： 	当前经过校正的测量值
*输入参数：		gx gy gz 三轴角速度 单位rad/s
				ax ay az 三轴加速度（在静态是为重力加速度的三轴投影，无单位，只需要比例关系）当需要加速度积分速度位移时需要单位
				mx my mz 三轴磁场强度 无单位 因为只需要比例关系 
*相关知识： //四元数
*******************************************************************************/
void AHRS_EKF_AHRSupdate(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz,float *roll,float *pitch,float *yaw)
{
	/*
	=============================================================================
	=============================================================================
	=============================================================================
	X = [q0 q1 q2 q3 w1 w2 w3]' 四元数 + 角速度偏移 认为两次采样中角速度偏移不变
	状态方程：
	X(k+1) = F*X(k)
	状态转移矩阵：
	F = |1_(4*4) + (dq/dt)*dt 		0_(4*3)	|
		|0_(3*4)				Eye_(3*3)	|(7*7)
	
	其中：	https://wenku.baidu.com/view/2f3d5bc0d5bbfd0a795673fa.html 【四元数微分方程的推导】
			https://wenku.baidu.com/view/b2f5ac27a5e9856a561260ce.html 【高阶四元数求导】《惯性导航》P302
				|0  -Wx -Wy  -Wz||q0|
	dq/dt = 1/2*|Wx  0   Wz  -Wy||q1|  Wx Wy Wz 是角速率 【此处为 1 阶微分】
				|Wy -Wz  0    Wx||q2|
				|Wz  Wy -Wx   0 ||q3|
	
	X(k+1) = 	F*X(k) = |1_(4*4) + (dq/dt)*dt 		 		0_(4*3)	|		*  	|q0 q1 q2 q3 q4 w1 w2 w3|'(7*1)	
						|0_(3*4)						Eye_(3*3)	|(7*7)
	=============================================================================
	=============================================================================
	=============================================================================
	Y = [ax ay az mx my mz]'    固联坐标系实测的重力加速度 + 固联坐标系实测的磁场强度 
	观测方程：
	Y(k+1) = H*X(k)
	观测转移矩阵：
			|	-q2*g			q3*g			-q0*g  			-q1*g			0	0  	0	|
			|	q1*g			q0*g  			q3*g  			q2*g			0	0  	0	|
	H = 2*	|	q0*g 	 		-q1*g			-q2*g 			q3*g			0	0  	0	|
			|bx*q0-bz*q2	bx*q1+bz*q3  	-bx*q2-bz*q0  -bx*q3+bz*q1			0	0  	0	|
			|-bx*q3+bz*q1	bx*q2+bz*q0  	bx*q1+bz*q3 	-bx*q0+bz*q2		0	0  	0	|
			|bx*q2+bz*q0	bx*q3-bz*q1  	bx*q0-bz*q2  	bx*q1+bz*q3			0	0  	0	|(6*7)
					
				|-q2*g				q3*g			-q0*g  			-q1*g		0	0  	0	|				|q0|
				|	q1*g			q0*g  			q3*g  			q2*g		0	0  	0	|				|q1|
	Y(k+1) =  2*|	q0*g 	 		-q1*g			-q2*g 			q3*g		0	0  	0	|			* |q2|
				|bx*q0-bz*q2	bx*q1+bz*q3  	-bx*q2-bz*q0  -bx*q3+bz*q1		0	0  	0	|				|q3|
				|-bx*q3+bz*q1	bx*q2+bz*q0  	bx*q1+bz*q3 	-bx*q0+bz*q2	0	0  	0	|				|w1|
				|bx*q2+bz*q0	bx*q3-bz*q1  	bx*q0-bz*q2  	bx*q1+bz*q3		0	0  	0	|(6*7)	|w2|
																																															|w3|(7*1)
	=============================================================================
	=============================================================================
	=============================================================================
	*/
	float norm;//模值
	float bx, bz;//地理真实磁场（用于根据姿态变化计算磁场预测值）
	float vx, vy, vz, wx, wy, wz;//加速度观测的预测值 和 磁场观测的预测值
	float g=9.79973;//当地重力加速度
	
	/*观测方程的系数矩阵的暂存值,需要用到的*/ 
	/*分别是加速度对四元数的偏导和磁场强度对四元数的偏导*/
	float Ha1,Ha2,Ha3,Ha4,Hb1,Hb2,Hb3,Hb4;
	float e1,e2,e3,e4,e5,e6;//误差值
	float halfT_EKF;//四元数微分中需要 T/2
	
	float q0q0 = q0*q0;	//四元素运算会用到的值 共 1+2+3+4=10个值
	float q0q1 = q0*q1;	//主要用于旋转矩阵的表示  以及  坐标系的转换
	float q0q2 = q0*q2;
	float q0q3 = q0*q3;
	float q1q1 = q1*q1;
	float q1q2 = q1*q2;
	float q1q3 = q1*q3;
	float q2q2 = q2*q2;
	float q2q3 = q2*q3;
	float q3q3 = q3*q3;   

	//坐标系为NED(北东地)-->xyz(右手坐标系)
	bx = 0.5500;// bx指向北
	bz = 0.8351; //bz指向地
	
	now = Tick_ekf;  //读取时间 32位定时器
	if(now < lastUpdate){//定时器溢出过了
		halfT_EKF = ((float)(now+(0xffffffff-lastUpdate))/2000000.0f);//发生时间溢出后
	}
	else	{
	  halfT_EKF = ((float)(now-lastUpdate)/2000000.0f);//得到时间并将单位us转换为单位为s
	}
	lastUpdate = now;	//更新时间
	
	norm = invSqrt(ax*ax + ay*ay + az*az);//加速度平方根分之一       
	ax = ax * norm*g;//加速度归一化
	ay = ay * norm*g;//加速度归一化
	az = az * norm*g;//加速度归一化

	norm = invSqrt(mx*mx + my*my + mz*mz);//磁力计平方根分之一            
	mx = mx * norm;//磁场强度归一化
	my = my * norm;//磁场强度归一化
	mz = mz * norm;//磁场强度归一化

	gx = gx-w1;//角速度 w1 w2 w3 是角速度测量误差
	gy = gy-w2;//角速度 w1 w2 w3 是角速度测量误差
	gz = gz-w3;//角速度 w1 w2 w3 是角速度测量误差
	/*****************************************************************************************************************************************/
	/*****************************************************************************************************************************************/
	/*****************************************************************************************************************************************/
	/*****************************************************************************************************************************************/
	/*****************************************************************************************************************************************/	
	/*****************************************************************************************************************************************
	黄小平 《卡尔曼滤波原理与应用》4.2.1节 P80页【EKF滤波流程】
	第2步：状态预测
	X(k|k-1)=FX(k-1)
	状态更新 一阶龙格库塔法更新四元数  四元数表示的是加速度的方向向量
					|0  -Wx -Wy  -Wz|		|q0|
	dq/dt =	1/2	*	|Wx  0   Wz  -Wy|	*	|q1|
					|Wy -Wz  0    Wx|		|q2|
					|Wz  Wy -Wx   0 |		|q3|
	*/
	q0 = q0 + (-q1*gx - q2*gy - q3*gz)*halfT_EKF;
	q1 = q1 + (q0*gx + q2*gz - q3*gy)*halfT_EKF;
	q2 = q2 + (q0*gy - q1*gz + q3*gx)*halfT_EKF;
	q3 = q3 + (q0*gz + q1*gy - q2*gx)*halfT_EKF;  
	/* 四元数归一*/
	norm = invSqrt(q0*q0 + q1*q1 + q2*q2 + q3*q3);
	q0 = q0 * norm;
	q1 = q1 * norm;
	q2 = q2 * norm;
	q3 = q3 * norm;
	/*****************************************************************************************************************************************
	黄小平 《卡尔曼滤波原理与应用》4.2.1节 P80页【EKF滤波流程】
	第3步：观测预测
	Y(k|k-1)=HX(k-1)
	原理参见第5步的旋转矩阵
	*/
	vx = 2*(q1q3 - q0q2)*g;//ax 的预测值
	vy = 2*(q0q1 + q2q3)*g;//ay 的预测值
	vz = (q0q0 - q1q1 - q2q2 + q3q3)*g;//az 的预测值
	wx = 2*bx*(0.5 - q2q2 - q3q3) + 2*bz*(q1q3 - q0q2);//mx 的预测值
	wy = 2*bx*(q1q2 - q0q3) + 2*bz*(q0q1 + q2q3);//my 的预测值
	wz = 2*bx*(q0q2 + q1q3) + 2*bz*(0.5 - q1q1 - q2q2); //mz 的预测值 	
	/*****************************************************************************************************************************************
	黄小平 《卡尔曼滤波原理与应用》4.2.1节 P80页【EKF滤波流程】
	第4步：状态转移矩阵赋值
	F = dF(X)/dX
	*/ 
	F[0]=1;			F[1]=-gx*halfT_EKF;	F[2]=-gz*halfT_EKF;	F[3]=-gz*halfT_EKF;	F[4]=0; 	F[5]=0; 	F[6]=0;
	F[7]=gx*halfT_EKF;	F[8]=1;			F[9]=gz*halfT_EKF;	F[10]=-gy*halfT_EKF;F[11]=0; 	F[12]=0; 	F[13]=0;
	F[14]=gy*halfT_EKF;	F[15]=-gz*halfT_EKF;F[16]=1;		F[17]=gx*halfT_EKF;	F[18]=0; 	F[19]=0;	F[20]=0;
	F[21]=gz*halfT_EKF;	F[22]=gy*halfT_EKF;	F[23]=-gx*halfT_EKF;F[24]=1;		F[25]=0; 	F[26]=0; 	F[27]=0;
	F[28]=0;		F[29]=0;		F[30]=0;		F[31]=0;		F[32]=1;	F[33]=0;	F[34]=0;
	F[35]=0;		F[36]=0;		F[37]=0;		F[38]=0;		F[39]=0;	F[40]=1;	F[41]=0;
	F[42]=0;		F[43]=0;		F[44]=0;		F[45]=0;		F[46]=0;	F[47]=0;	F[48]=1;
	/*****************************************************************************************************************************************
	黄小平 《卡尔曼滤波原理与应用》4.2.1节 P80页【EKF滤波流程】
	第5步：观测转移矩阵赋值
	H = dH(X)/dX
	=============================================================================
	=============================重力加速度======================================
	=============================================================================
	根据四元数表示的姿态角计算重力加速度在固联坐标系下的预测值
	q0q0 + q1q1 + q2q2 + q3q3 = 1
	Reb表示RotationEarthFromBody 从固联坐标系转为地球坐标系(右手坐标系)
	Rbe表示RotationBodyFromEarth 从地球坐标系转为固联坐标系(右手坐标系)
				|q0q0+q1q1-q2q2-q3q3  	2(q1q2+q0q3)  				2(q1q3-q0q2)| 
	Rbe = 		|2(q1q2-q0q3)   		q0q0-q1q1+q2q2-q3q3  		2(q2q3+q0q1)| 【这是右乘矩阵： b' = R(rigth)*a'】
				|2(q1q3+q0q2)  			2(q2q3-q0q1)  		 q0q0-q1q1-q2q2+q3q3|	惯性导航P295页所列的为Reb的右乘矩阵（固联坐标系到地球坐标系）		
	
	[Gx Gy Gz]'=Rbe*[0 0 g]'
	
	因此：（其中vx vy vz为重力加速度在固联坐标系的投影）
	vx = 2*(q1q3 - q0q2)*g;
	vy = 2*(q0q1 + q2q3)*g;
	vz = (q0q0 - q1q1 - q2q2 + q3q3)*g;
	=============================================================================
	===============================磁场强度======================================
	=============================================================================
	同理：根据四元数表示的姿态角计算磁场强度在固联坐标系下的预测值
	Reb表示RotationEarthFromBody 从固联坐标系转为地球坐标系(右手坐标系)
	Rbe表示RotationBodyFromEarth 从地球坐标系转为固联坐标系(右手坐标系)
				|q0q0+q1q1-q2q2-q3q3  	2(q1q2+q0q3)  					2(q1q3-q0q2)|
	Rbe = 		|2(q1q2-q0q3)   		q0q0-q1q1+q2q2-q3q3  			2(q2q3+q0q1)|【这是右乘矩阵： b' = R(rigth)*a'】
				|2(q1q3+q0q2)  			2(q2q3-q0q1)  			 q0q0-q1q1-q2q2+q3q3| 惯性导航P295页所列的为Reb的右乘矩阵（固联坐标系到地球坐标系）			
	[Mx My Mz]'=Rbe*[bx 0 bz]'
	
	因此：(其中 wx wy wz 为磁场强度在固联坐标系的投影)
	wx = 2*bx*(0.5 - q2q2 - q3q3) + 2*bz*(q1q3 - q0q2);
	wy = 2*bx*(q1q2 - q0q3) + 2*bz*(q0q1 + q2q3);
	wz = 2*bx*(q0q2 + q1q3) + 2*bz*(0.5 - q1q1 - q2q2);
	=============================================================================
	=============================================================================
	=============================================================================
	则有：(充当坐标变换)
			|@ax/@q0		@ax/@q1			@ax/@q2  		@ax/@q3			@ax/@w1			@ax/@w2  		@ax/@w3	|
			|@ay/@q0		@ay/@q1  		@ay/@q2  		@ay/@q3			@ay/@w1			@ay/@w2  		@ay/@w3	|
	 H =  	|@az/@q0 		@az/@q1			@az/@q2 		@az/@q3			@az/@w1			@az/@w2  		@az/@w3	|
			|@mx/@q0		@mx/@q1  		@mx/@q2  		@mx/@q3			@mx/@w1			@mx/@w2  		@mx/@w3	|
			|@my/@q0		@my/@q1  		@my/@q2  		@my/@q3			@my/@w1			@my/@w2  		@my/@w3	|
			|@mz/@q0		@mz/@q1  		@mz/@q2  		@mz/@q3			@mz/@w1			@mz/@w2  		@mz/@w3	|

			|	-q2*g			q3*g			-q0*g  			-q1*g		0	0  	0	|
			|	q1*g			q0*g  			q3*g  			q2*g		0	0  	0	|
		= 2*|	q0*g 	 		-q1*g			-q2*g 			q3*g		0	0  	0	|
			|bx*q0-bz*q2	bx*q1+bz*q3  	-bx*q2-bz*q0  -bx*q3+bz*q1		0	0  	0	|
			|-bx*q3+bz*q1	bx*q2+bz*q0  	bx*q1+bz*q3 	-bx*q0+bz*q2	0	0  	0	|
			|bx*q2+bz*q0	bx*q3-bz*q1  	bx*q0-bz*q2  	bx*q1+bz*q3		0	0  	0	|				
	=============================================================================
	=============================================================================
	=============================================================================
	*/
	Ha1=2*-q2*g; /*对观测方程系数矩阵相关偏导数值预先计算*/
	Ha2=2*q3*g;/*对观测方程系数矩阵相关偏导数值预先计算*/
	Ha3=2*-q0*g;/*对观测方程系数矩阵相关偏导数值预先计算*/
	Ha4=2*q1*g;	/*对观测方程系数矩阵相关偏导数值预先计算*/
	Hb1=2*(bx*q0-bz*q2);/*对观测方程系数矩阵相关偏导数值预先计算*/
	Hb2=2*(bx*q1+bz*q3);/*对观测方程系数矩阵相关偏导数值预先计算*/
	Hb3=2*(-bx*q2-bz*q0);/*对观测方程系数矩阵相关偏导数值预先计算*/
	Hb4=2*(-bx*q3+bz*q1);/*对观测方程系数矩阵相关偏导数值预先计算*/
	H[0]=Ha1;	H[1]= Ha2;	H[2]= Ha3;	H[3]= Ha4;
	H[7]= Ha4;	H[8]=-Ha3;	H[9]= Ha2;	H[10]=-Ha1;
	H[14]=-Ha3;	H[15]=-Ha4;	H[16]= Ha1;	H[17]= Ha2;
	H[21]= Hb1;	H[22]= Hb2;	H[23]= Hb3;	H[24]= Hb4;
	H[28]= Hb4;	H[29]=-Hb3;	H[30]= Hb2;	H[31]=-Hb1;
	H[35]=-Hb3;	H[36]=-Hb4;	H[37]= Hb1;	H[38]= Hb2;
	/****************************************************************************************************************************************
	黄小平 《卡尔曼滤波原理与应用》4.2.1节 P80页【EKF滤波流程】
	第6步：求协方差矩阵
	P(k|k-1) = F(k)P(k-1|k-1)F' + Q
	*/
	MatrixMultiply(F,7,7,P,7,7,A );	//A=F*P 
	MatrixTranspose(F,7,7,Ft);	  //F转置F'
	MatrixMultiply(A,7,7,Ft,7,7,B); // B=F*P*F'
	MatrixAdd(B,Q,P1,7,7 );
	/****************************************************************************************************************************************
	*黄小平 《卡尔曼滤波原理与应用》4.2.1节 P80页【EKF滤波流程】
	*第7步：求卡尔曼滤波增益
	*K(k) = P(k|k-1)H(k)'[H(k)P(k|k-1)H' + R]
	*/
	//[H(k)P(k|k-1)H' + R]
	MatrixTranspose(H,6,7,Ht);	  //Ht = H'
	MatrixMultiply(P1,7,7,Ht,7,6,E ); //E=P*H'
	MatrixMultiply(H,6,7,E,7,6,F1 ); //	 F1=H*P*H'
	MatrixAdd(F1,R,X,6,6 );           //X=F1+R 
	//K(k) = P(k|k-1)H(k)'[H(k)P(k|k-1)H' + R] 
	//卡尔曼滤波器UD分解滤波算法的改进 http://www.cnki.com.cn/Article/CJFDTotal-YCYK200501002.htm
	UD(X,6,U1,D1);	   //求逆前做UD分解 防止发散
	MatrixTranspose(U1,6,6,U1t);//U1
	MatrixMultiply(U1,6,6,D1,6,6,X1); //X1=U1*D1
	MatrixMultiply(X1,6,6,U1t,6,6,X2); //X2=U1*D1*U1t  （还原矩阵）
	MatrixInverse(X2,6,X2);	//对X2求逆  
	MatrixMultiply(E,7,6,X2,6,6,K ); //增益K   7*6
	
	/****************************************************************************************************************************************
	黄小平 《卡尔曼滤波原理与应用》4.2.1节 P80页【EKF滤波流程】
	第8步：更新状态 X （求更新量）
	X(k) = X(k|k-1)+K[(Y(k)-HX(k-1)]
	*/
	/*计算观测值和预测值之间的偏差 为卡尔曼滤波提供依据*/
	e1=ax-vx;e2=ay-vy;e3=az-vz;//加速度偏差
	e4=mx-wx;e5=my-wy;e6=mz-wz;//磁场偏差
	/*给误差矩阵 T 赋值*/	
	T[0]=e1;T[1]=e2;T[2]=e3;T[3]=e4;T[4]=e5;T[5]=e6;
	MatrixMultiply(K,7,6,T,6,1,Y );   //Y=K*(Z-Y)	7*1 其中 T = Z(观测) - Y(预测)
	//根据变化量更新状态向量(四元数和固联坐标系下的磁场)
	//X = [q0 q1 q2 q3 w1 w2 w3]'	
	q0= q0+Y[0];//更新四元数
	q1= q1+Y[1];//更新四元数
	q2= q2+Y[2];//更新四元数
	q3= q3+Y[3];//更新四元数
	w1= w1+Y[4];//更新角速度偏移
	w2= w2+Y[5];//更新角速度偏移
	w3= w3+Y[6];//更新角速度偏移

  *pitch = asin(-2 * q1 * q3 + 2 * q0 * q2)* 57.3;	// pitch
  *roll  = atan2(2 * q2 * q3 + 2 * q0 * q1, -2 * q1 * q1 - 2 * q2 * q2 + 1)* 57.3;	// roll
  *yaw   = atan2(2*(q1*q2 + q0*q3),q0*q0+q1*q1-q2*q2-q3*q3) * 57.3;	//yaw
	/*****************************************************************************************************************************************
	黄小平 《卡尔曼滤波原理与应用》4.2.1节 P80页【EKF滤波流程】
	第9步：更新协方差
	K(k) = [I-K(k)H(k)]P(k|k-1)
	*/
	MatrixMultiply(K,7,6,H,6,7,Z); //Z= K*H		7*7
	MatrixSub(I,Z,O,7,7 );//O=I-K*H
	MatrixMultiply(O,7,7,P1,7,7,P);

	/*四元数归一化 保证下一次使用的四元数是单位四元数*/
	norm = invSqrt(q0*q0 + q1*q1 + q2*q2 + q3*q3);
	q0 = q0 * norm;
	q1 = q1 * norm;
	q2 = q2 * norm;
	q3 = q3 * norm;
}

