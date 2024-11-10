/*一、卡尔曼*/
/*二、模糊控制PID-------在Esp8266学习处框图*/



volatile变量只接受赋值并没有给其他变量赋值或传函数很可能被编译器优化掉


/*一、卡尔曼*/
kalman理论
kalman:前一时刻估计值和观测值->现时刻最优估计值  
噪声:模型估计值与实际值的误差
最优估计:当前目标位置-滤波,将来目标位置-预测,过去目标位置-插值或平滑
逻辑过程:传感器采集+数学模型->公式求出最优->拿较传感器与数学模型对比之->公式生成权重
传感器采集+数学模型->权重->公式求出最优->拿较传感器与数学模型对比之->公式生成权重...

直接用原始数据加速度计算出来的姿态角是不能用的,因为实际上,mpu6050输出的加速度值易受外界干扰，
比如运动中的加速度和飞机在振荡产生的加速度,噪声较大,所以我们是无法直接的运用加速度计算出来的姿态角，
需要我们对数据进行一定的处理（滤波）才能使用。

1)加速度计滤波->姿态解算      2)姿态解算(嵌入)->kalman滤波

1、入门 
	1)适用系统:叠加性,齐次性->线性+高斯噪声=>线性高斯系统  ps:《信号与系统》-奥本海默
	2)宏观意义:滤波即加权   信号*1+噪声*0----估计值+观测值=>卡尔曼滤波
	3)本质:传感器数量不够或者精度不够,滤除噪声,平滑信号
2、进阶
	离散系统模型 
	状态方程->A- B  C- u-输入 w-过程噪声   如:位移X
	观测方程->y-观测值 v-观测噪声(传感器)
	方差(因符合正态分布)->一维方差Qk Rk X   二维方差X1,X2 Qk1 Rk1 Qk2 Rk2且相互影响(协方差矩阵)   多维方差
	超参数->Q-过程噪声方差 R-观测噪声方差  
	当前的最优估计值=先验估计值+当前观测值加权
3、再进阶
	预测*2=状态+协方差方程   更新修正*3=当前状态+协方差+卡尔曼增益方程
	H-测量状态量的权值矩阵  K-卡尔曼增益  P-协方差矩阵  F-状态转移矩阵  
	更信任观测值-K增-表示对测量值的信任程度  调Q-运动模型  R-传感器  
4、精通
	实战

MPU6050:
	四元数法-YAW,PITCH,ROLL  思想:在方向余弦基础上反解欧拉角
	一阶互补-PITCH,ROLL   思想:给加速度和陀螺仪不同的权值，把它们结合到一起，进行修正
	卡尔曼滤波-PITCH,ROLL 思想:2维卡尔曼滤波,预测和修正加权
	kalman前提:当前的测量值和前一个采样周期的估计值才能进行状态估计(比如加速度计得测量值,陀螺仪积累得估计值)
MPU9250:
    ACC:+-2G // 4G范围,16bit-> 0.0006 m/s²;范围和分辨率
    DLPF:数字低通滤波器:<460Hz可以通过,延迟1.94ms是指从传感器测量到数据输出之间的时间差。
    噪声-滤波器-带宽-延迟

    GYRO:250dps // 500dps范围,16bit-> 0.0078125°/s
    DLPF:数字低通滤波器:<8800Hz可以通过,延迟0.064ms是指从传感器测量到数据输出之间的时间差。
    
    // 加速度计在低功耗模式下的输出数据速率为 0.24 Hz
    // 较高的 ODR 会增加数据更新频率，但功耗也会相应增加
姿态表示:四元数->方向余弦矩阵(旋转矩阵)、欧拉角->方向余弦矩阵(旋转矩阵)   四元数成为在三维空间中表示旋转的一个强大工具,避免万向死锁
Mahony算法:
EKF算法:
TAT-1->代表矩阵变换(参考系变换)

//1. 结构体类型定义
typedef struct 
{
    float LastP;//上次估算协方差 初始化值为0.02
    float Now_P;//当前估算协方差 初始化值为0
    float out;//卡尔曼滤波器输出 初始化值为0
    float Kg;//卡尔曼增益 初始化值为0
    float Q;//过程噪声协方差 初始化值为0.001
    float R;//观测噪声协方差 初始化值为0.543
}KFP；//Kalman Filter parameter

//2. 以高度为例 定义卡尔曼结构体并初始化参数
KFP KFP_height={0.02,0,0,0,0.001,0.543};
/**
 *卡尔曼滤波器
 *@param KFP *kfp 卡尔曼结构体参数
 *   float input 需要滤波的参数的测量值（即传感器的采集值）
 *@return 滤波后的参数（最优值）
 */
 float kalmanFilter(KFP *kfp,float input)
 {
     //预测协方差方程：k时刻系统估算协方差 = k-1时刻的系统协方差 + 过程噪声协方差
     kfp->Now_P = kfp->LastP + kfp->Q;
     //卡尔曼增益方程：卡尔曼增益 = k时刻系统估算协方差 / （k时刻系统估算协方差 + 观测噪声协方差）
     kfp->Kg = kfp->Now_P / (kfp->NOw_P + kfp->R);
     //更新最优值方程：k时刻状态变量的最优值 = 状态变量的预测值 + 卡尔曼增益 * （测量值 - 状态变量的预测值）
     kfp->out = kfp->out + kfp->Kg * (input -kfp->out);//因为这一次的预测值就是上一次的输出值
     //更新协方差方程: 本次的系统协方差付给 kfp->LastP 威下一次运算准备。
     kfp->LastP = (1-kfp->Kg) * kfp->Now_P;
     return kfp->out；
 }
/**
 *调用卡尔曼滤波器 实践
 */
int height;
int kalman_height=0;
kalman_height = kalmanFilter(&KFP_height,(float)height);


//卡尔曼滤波的基本思想是综合利用上一次的状态和测量值来对物理量的状态进行预测估计。
typedef struct{
    float filterValue;//滤波后的值
    float kalmanGain;//Kalamn增益
    float A;//状态矩阵
    float H;//观测矩阵
    float Q;//状态矩阵的方差
    float R;//观测矩阵的方差
    float P;//预测误差
    float B;
    float u;
}KalmanInfo;
void Kalm::initKalmanFilter(KalmanInfo *info)
{
    info->A = 1;
    info->H = 1;
    info->P = 0.1;
    info->Q = 0.05;
    info->R = 0.1;
    info->B = 0.1;
    info->u = 0;
    info->filterValue = 0;
}
float Kalm::kalmanFilterFun(KalmanInfo *info, float new_value)
{
    float predictValue = info->A*info->filterValue+info->B*info->u;//计算预测值
    info->P = info->A*info->A*info->P + info->Q;//求协方差
    info->kalmanGain = info->P * info->H /(info->P * info->H * info->H + info->R);//计算卡尔曼增益
    info->filterValue = predictValue + (new_value - predictValue)*info->kalmanGain;//计算输出的值
    info->P = (1 - info->kalmanGain* info->H)*info->P;//更新协方差
    return info->filterValue;
}





/*二、模糊控制PID-------在Esp8266学习处框图*/
/*定义结构体和公用体*/
typedef struct
{
  float setpoint;               /*设定值*/
  float kp;                     /*比例系数*/
  float ki;                     /*积分系数*/
  float kd;                     /*微分系数*/
  float lasterror;              /*前一拍偏差*/
  float preerror;               /*前两拍偏差*/
  float deadband;               /*死区*/
  float output;                 /*输出值*/
  float result;                 /*物理量输出值*/
  float maximum;                /*输出值的上限*/
  float minimum;                /*输出值的下限*/
 
  float maxdKp;                 /*Kp增量的最大限值*/
  float mindKp;                 /*Kp增量的最小限值*/
  float qKp;                    /*Kp增量的影响系数*/
  float maxdKi;                 /*Ki增量的最大限值*/
  float mindKi;                 /*Ki增量的最小限值*/
  float qKi;                    /*Ki增量的影响系数*/
  float maxdKd;                 /*Kd增量的最大限值*/
  float mindKd;                 /*Kd增量的最小限值*/
  float qKd;                    /*Kd增量的影响系数*/
}FUZZYPID;
/*线性量化操作函数,论域{-6，-5，-4，-3，-2，-1，0，1，2，3，4，5，6}*/
static void LinearQuantization(FUZZYPID *vPID,float pv,float *qValue)
{
  float thisError;
  float deltaError;
 
  thisError=vPID->setpoint-pv;                  //计算偏差值
  deltaError=thisError-vPID->lasterror;         //计算偏差增量
 
  qValue[0]=6.0*thisError/(vPID->maximum-vPID->minimum);
  qValue[1]=3.0*deltaError/(vPID->maximum-vPID->minimum);//线性方式量化
}
/*隶属度计算函数*/
static void CalcMembership(float *ms,float qv,int * index)
{
  if((qv>=-NB)&&(qv<-NM))
  {
    index[0]=0;
    index[1]=1;
    ms[0]=-0.5*qv-2.0;  //y=-0.5x-2.0
    ms[1]=0.5*qv+3.0;   //y=0.5x+3.0  三角隶属度函数(一次函数)
  }
  else if((qv>=-NM)&&(qv<-NS))
  {
    index[0]=1;
    index[1]=2;
    ms[0]=-0.5*qv-1.0;  //y=-0.5x-1.0
    ms[1]=0.5*qv+2.0;   //y=0.5x+2.0
  }
  else if((qv>=-NS)&&(qv<ZO))
  {
    index[0]=2;
    index[1]=3;
    ms[0]=-0.5*qv;      //y=-0.5x
    ms[1]=0.5*qv+1.0;   //y=0.5x+1.0
  }
  else if((qv>=ZO)&&(qv<PS))
  {
    index[0]=3;
    index[1]=4;
    ms[0]=-0.5*qv+1.0;  //y=-0.5x+1.0
    ms[1]=0.5*qv;       //y=0.5x
  }
  else if((qv>=PS)&&(qv<PM))
  {
    index[0]=4;
    index[1]=5;
    ms[0]=-0.5*qv+2.0;  //y=-0.5x+2.0
    ms[1]=0.5*qv-1.0;   //y=0.5x-1.0
  }
  else if((qv>=PM)&&(qv<=PB))
  {
    index[0]=5;
    index[1]=6;
    ms[0]=-0.5*qv+3.0;  //y=-0.5x+3.0
    ms[1]=0.5*qv-2.0;   //y=0.5x-2.0
  }
}
/*解模糊化操作,根据具体的量化函数和隶属度函数调整*/
static void FuzzyComputation (FUZZYPID *vPID,float pv,float *deltaK)
{
  float qValue[2]={0,0};        //偏差及其增量的量化值
  int indexE[2]={0,0};          //偏差隶属度索引
  float msE[2]={0,0};           //偏差隶属度
  int indexEC[2]={0,0};         //偏差增量隶属度索引
  float msEC[2]={0,0};          //偏差增量隶属度
  float qValueK[3];
 
  LinearQuantization(vPID,pv,qValue);//量化函数
 
  CalcMembership(msE,qValue[0],indexE);//隶属度函数
  CalcMembership(msEC,qValue[1],indexEC);
 //模糊规则表
  qValueK[0]=msE[0]*(msEC[0]*ruleKp[indexE[0]][indexEC[0]]+msEC[1]*ruleKp[indexE[0]][indexEC[1]])
            +msE[1]*(msEC[0]*ruleKp[indexE[1]][indexEC[0]]+msEC[1]*ruleKp[indexE[1]][indexEC[1]]);
  qValueK[1]=msE[0]*(msEC[0]*ruleKi[indexE[0]][indexEC[0]]+msEC[1]*ruleKi[indexE[0]][indexEC[1]])
            +msE[1]*(msEC[0]*ruleKi[indexE[1]][indexEC[0]]+msEC[1]*ruleKi[indexE[1]][indexEC[1]]);
  qValueK[2]=msE[0]*(msEC[0]*ruleKd[indexE[0]][indexEC[0]]+msEC[1]*ruleKd[indexE[0]][indexEC[1]])
            +msE[1]*(msEC[0]*ruleKd[indexE[1]][indexEC[0]]+msEC[1]*ruleKd[indexE[1]][indexEC[1]]);
 
  deltaK[0]=LinearRealization(vPID->maxdKp,vPID->mindKp,qValueK[0]);
  deltaK[1]=LinearRealization(vPID->maxdKi,vPID->mindKi,qValueK[1]);
  deltaK[2]=LinearRealization(vPID->maxdKd,vPID->mindKd,qValueK[2]);//∆Kp、∆Ki、∆Kd
}


/* 三、拓展卡尔曼滤波EKF */
/**************************实现函数********************************************
*函数原型:	 	void AHRS_AHRSupdate
*功　　能:	 	更新AHRS 更新四元数 
*输入参数： 	当前经过校正的测量值
*输入参数：		gx gy gz 三轴角速度 单位rad/s
				ax ay az 三轴加速度（在静态是为重力加速度的三轴投影，无单位，只需要比例关系）当需要加速度积分速度位移时需要单位
				mx my mz 三轴磁场强度 无单位 因为只需要比例关系 
*相关知识： //四元数
				①秦永元 《惯性导航 9.2节》
				②https://blog.csdn.net/shenshikexmu/article/details/53608224?locationNum=8&fps=1  【讲了左乘矩阵和右乘矩阵的差别】
			//扩展卡尔曼滤波
				①黄小平 《卡尔曼滤波原理与应用 4.2.1节》【EKF滤波流程】
*******************************************************************************/

void AHRS_AHRSupdate(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz)
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
	float halfT;//四元数微分中需要 T/2
	
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
	
	now = micros();  //读取时间 32位定时器
	if(now<lastUpdate){//定时器溢出过了
		halfT=((float)(now+(0xffffffff-lastUpdate))/2000000.0f);//发生时间溢出后
	}
	else	{
	halfT=((float)(now-lastUpdate)/2000000.0f);//得到时间并将单位us转换为单位为s
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

	gx=gx-w1;//角速度 w1 w2 w3 是角速度测量误差
	gy=gy-w2;//角速度 w1 w2 w3 是角速度测量误差
	gz=gz-w3;//角速度 w1 w2 w3 是角速度测量误差
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
	q0 = q0 + (-q1*gx - q2*gy - q3*gz)*halfT;
	q1 = q1 + (q0*gx + q2*gz - q3*gy)*halfT;
	q2 = q2 + (q0*gy - q1*gz + q3*gx)*halfT;
	q3 = q3 + (q0*gz + q1*gy - q2*gx)*halfT;  
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
	F[0]=1;			F[1]=-gx*halfT;	F[2]=-gz*halfT;	F[3]=-gz*halfT;	F[4]=0; 	F[5]=0; 	F[6]=0;
	F[7]=gx*halfT;	F[8]=1;			F[9]=gz*halfT;	F[10]=-gy*halfT;F[11]=0; 	F[12]=0; 	F[13]=0;
	F[14]=gy*halfT;	F[15]=-gz*halfT;F[16]=1;		F[17]=gx*halfT;	F[18]=0; 	F[19]=0;	F[20]=0;
	F[21]=gz*halfT;	F[22]=gy*halfT;	F[23]=-gx*halfT;F[24]=1;		F[25]=0; 	F[26]=0; 	F[27]=0;
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
	则有：
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
	MatrixAdd( B,Q,P1,7,7 );
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
	MatrixInverse(X2,6,0);	//对X2求逆  
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


