#include "bsp_math.h"

/**
 * @brief: 返回(limit_down,limit_up)间值
 * @param {float} input
 * @param {float} limit_down
 * @param {float} limit_up
 * @return {*}
 */
float Limit_up_and_down(float input, float limit_down, float limit_up)
{
    float output = 0;
    output = (input < limit_down) ? limit_down : ((input > limit_up) ? limit_up : input);

    return output;
}

/**
 * @brief: 一阶线性低通滤波
 * @param {foc_handler} *foc_data
 * @param {float} dt
 * @return {*}
 */
float Low_Pass_Filter(lpf_filter *lpf_t)
{
    float lpf_speed = 0;
    lpf_t->lpf_parm = 0.8;
    lpf_speed = (lpf_t->lpf_parm * lpf_t->value_lpf) +
                (1.0f - lpf_t->lpf_parm) * lpf_t->value_lpf_prev;
    lpf_t->value_lpf_prev = lpf_speed;

    return lpf_speed;
}

