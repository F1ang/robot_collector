# robot_collector
Robot collector is elegant

# Balance Car
  1、运算时间
    tim7:2khz中断(0.5ms),获取imu数据   
    2khz/20 = 100hz(10ms),计算编码速度,进行pid运算

    采用10ms 获取imu数据 进行PID控制 
    注:如果采用MPU6050,采样周期可以设200hz(5ms),在imu_task任务中进行数据融合,然后得到姿态角和角速度数据,在move_task()获取相对应数据即可
    本文采用了外置的IMU.
  2、数据单位
    (1)姿态角
    roll:2.63 pitch:9.61  yaw:-5.33
    roll:3.26 pitch:18.79 yaw:-5.59
    (2)角速度
    X:0.00 Y:0.24 Z:-0.12
    X:4.15 Y:62.01 Z:-2.56
    X:-5.80 Y:-101.20 Z:3.11
  3、PID参数
    #define SPEED_KP 0.22
    float  Vertical_Kp = -370; 
    float  Vertical_Kd = -25; 

    float Velocity_Kp = -SPEED_KP;         
    float Velocity_Ki = -SPEED_KP/200;   
  4、编码器数据
    float Machine_Angle = 3.01;	//机械角度中值，自平衡的角度值 
    short encoder_delta[2];	// [-42,42]   超速-189 -177
  5、可自行优化
    (1)跌倒输出清零
    (2)遥控+转向环


