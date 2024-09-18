/*
 * @Description: 
 * @Author: YiPei_Fang
 * @Date: 2022-11-08 14:44:13
 */
#ifndef _Struct_Init_H
#define _Struct_Init_H

#define PID1_Init(kp,ki,kd,integral_separation, integral_Max, max_out,closed_loop_dead, is_integral_separation, is_Closed_loop_dead)\
            {\
                .Kp = kp, \
                .Ki = ki, \
                .Kd = kd, \
                .Error[0] = 0, \
                .Error[1] = 0, \
                .Error[2] = 0, \
                .Integral = 0, \
                .Integral_separation = integral_separation, \
                .Integral_Max = integral_Max, \
                .Max_out = max_out, \
                .Closed_loop_dead = closed_loop_dead, \
                .Is_integral_separation = is_integral_separation, \
                .Is_Closed_loop_dead = is_Closed_loop_dead \
            }


typedef struct 
{
     /* data */
    float Kp,Ki,Kd;//kp ki kd
    int Error[3];//上上次last_last 上次last 现在now
    int Integral,Integral_separation,Integral_Max,Max_out,Closed_loop_dead;// 积分项 积分分离 积分限幅 输出限幅 闭环死区
    unsigned int Is_integral_separation,Is_Closed_loop_dead;//是否积分分离 是否闭环死区
}PID1;
    
typedef struct 
{
    /* data */
    int Speed,Angle;
}Measture;


/* 类型函数+指针 */
typedef void (*system_irq_handler_t)  (unsigned int giccIar,void *param);

/* 中断处理函数结构体 */
typedef struct _sys_irq_handle
{
    system_irq_handler_t irqHandler; /* 中断处理函数 */
    void *userParam; /* 中断处理函数参数 */
} sys_irq_handle_t;

//函数表(数组)->注册函数->调用注册函数

#define MINI_ERROR_COM 10
#define MINI_ERROR_UV 11
enum error_state_bool_m         // 回调函数返回类型
{
  error_off = 0,
  error_on,
};

struct app_error_code_t
{
  enum error_state_bool_m (*callback)(void);  // 结构体 地址指针 回调函数(无参)
};

struct mid_event_list_t { // 链表查询check_error_handle[],可以提高效率
  mid_event_list_t *next;
  mid_event_list_t *prev;
  int src_id;
  int dst_id;
  int cmd;
}

#endif

/*
    函数指针



*/




