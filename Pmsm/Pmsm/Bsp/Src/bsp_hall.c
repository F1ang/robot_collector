#include "bsp_hall.h"

bool RatioDec;           // 捕获psc减小
bool RatioInc;           // 捕获psc增大
bool DoRollingAverage;   // 滑动均值滤波
bool InitRollingAverage; // 初始化滑动均值滤波
bool HallTimeOut;        // 捕获超时

uint16_t hCaptCounter;    // 捕获计数器
uint8_t bGP1_OVF_Counter; // 捕获溢出计数器
uint8_t bSpeedFIFO_Index; // 速度BUFF索引

 int16_t hElectrical_Angle;
 int16_t hRotorFreq_dpp;
SpeedMeas_s SensorPeriod[HALL_SPEED_FIFO_SIZE]; // 滑动均值滤波数组

static int8_t bSpeed;

/**
 * @brief: HALL捕获中断和溢出更新中断
 * @return {*} : None
 */
void BSP_Hall_Init(void)
{
    HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_1); // TIM_IT_CC1
    HAL_TIM_Base_Start_IT(&htim3);              // TIM_IT_UPDATE
}

/**
 * @brief: 捕获次数清0
 * @return {*} : None
 */
void HALL_ClrCaptCounter(void)
{
    hCaptCounter = 0;
}

/**
 * @brief: CC1使能、HALL捕获溢出/非溢出处理标志
 * @return {*} : None
 */
void HALL_InitHallMeasure(void)
{
    HAL_TIM_IC_Stop_IT(&htim3, TIM_IT_CC1);

    /* HALL捕获溢出/溢出处理psc、cap、dir */
    RatioDec = FALSE;
    RatioInc = FALSE;
    DoRollingAverage = FALSE;
    InitRollingAverage = FALSE;
    HallTimeOut = FALSE;

    hCaptCounter = 0;
    bGP1_OVF_Counter = 0;

    /* 滑动均值滤波 */
    for (bSpeedFIFO_Index = 0; bSpeedFIFO_Index < HALL_SPEED_FIFO_SIZE;
         bSpeedFIFO_Index++) {
        SensorPeriod[bSpeedFIFO_Index].hCapture = U16_MAX;
        SensorPeriod[bSpeedFIFO_Index].hPrscReg = HALL_MAX_RATIO;
        SensorPeriod[bSpeedFIFO_Index].bDirection = POSITIVE;
    }

    // First measurement will be stored in the 1st array location
    bSpeedFIFO_Index = HALL_SPEED_FIFO_SIZE - 1;

    // Re-initialize partly the timer
    __HAL_TIM_SET_PRESCALER(&htim3, HALL_MAX_RATIO - 1);

    HALL_ClrCaptCounter();

    __HAL_TIM_SET_COUNTER(&htim3, HALL_COUNTER_RESET);

    BSP_Hall_Init();
}

/**
 * @brief: 预定位电角度
 * @return {*} : None
 */
void HALL_Init_Electrical_Angle(void) // 预定位
{
    uint8_t hall_sec = 0;
    hall_sec = HALL_U_VALUE << 2 | HALL_V_VALUE << 1 | HALL_W_VALUE << 0;
    switch (hall_sec) {
    case STATE_5:
        hElectrical_Angle = (int16_t)(S16_PHASE_SHIFT + S16_60_PHASE_SHIFT / 2);
        break;

    case STATE_1:
        hElectrical_Angle = (int16_t)(S16_PHASE_SHIFT + S16_60_PHASE_SHIFT +
                                      S16_60_PHASE_SHIFT / 2);
        break;
    case STATE_3:
        hElectrical_Angle = (int16_t)(S16_PHASE_SHIFT + S16_120_PHASE_SHIFT +
                                      S16_60_PHASE_SHIFT / 2);
        break;
    case STATE_2:
        hElectrical_Angle = (int16_t)(S16_PHASE_SHIFT - S16_120_PHASE_SHIFT -
                                      S16_60_PHASE_SHIFT / 2);
        break;
    case STATE_6:
        hElectrical_Angle = (int16_t)(S16_PHASE_SHIFT - S16_60_PHASE_SHIFT -
                                      S16_60_PHASE_SHIFT / 2);
        break;
    case STATE_4:
        hElectrical_Angle = (int16_t)(S16_PHASE_SHIFT - S16_60_PHASE_SHIFT / 2);
        break;
    default:
        break;
    }
}

/**
 * @brief: 电角度校准
 * @return {*} : dpp
 */
int16_t HALL_GetElectricalAngle(void)
{
    return (hElectrical_Angle);
}

/**
 * @brief: 电角度获取
 * @return {*} : None
 */
void HALL_IncElectricalAngle(void)
{
    static int16_t hPrevRotorFreq = 0;

    if (hRotorFreq_dpp != HALL_MAX_PSEUDO_SPEED) {
        hElectrical_Angle += hRotorFreq_dpp; // 1hz鐨勭數瑙掑害
        hPrevRotorFreq = hRotorFreq_dpp;
    } else {
        hElectrical_Angle += hPrevRotorFreq;
    }
}

/**
 * @brief: 清空捕获超时
 * @return {*} : None
 */
void HALL_ClrTimeOut(void)
{
    HallTimeOut = FALSE;
}

/**
 * @brief: 捕获的次数
 * @return {*} : 捕获的次数
 */
uint16_t HALL_GetCaptCounter(void)
{
    return (hCaptCounter);
}

/**
 * @brief: 清dpp
 * @return {*} : None
 */
void HALL_ClearRotorFreq_Dpp(void)
{
    hRotorFreq_dpp = 0;
}

/**
 * @brief: 转子转速
 * @return {*} : 单位0.1hz
 */
int16_t HALL_SetRotorFreq_Dpp(void)
{
    return hRotorFreq_dpp;
}

/**
 * @brief: 滑动均值滤波速度0.1hz
 * @return {*} : PeriodMeas_s
 */
PeriodMeas_s GetAvrgHallPeriod(void)
{
    uint32_t wFreqBuffer, wAvrgBuffer, wIndex;
    PeriodMeas_s PeriodMeasAux;

    wAvrgBuffer = 0;

    for (wIndex = 0; wIndex < HALL_SPEED_FIFO_SIZE; wIndex++) {
        // Disable capture interrupts to have presc and capture of the same period
        HAL_TIM_Base_Stop_IT(&htim3);              // TIM_IT_UPDATE
        HAL_TIM_IC_Stop_IT(&htim3, TIM_CHANNEL_1); // TIM_IT_CC1

        wFreqBuffer = SensorPeriod[wIndex].hCapture;
        wFreqBuffer *= (SensorPeriod[wIndex].hPrscReg + 1);

        HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_1); // TIM_IT_CC1
        HAL_TIM_Base_Start_IT(&htim3);              // TIM_IT_UPDATE
        wAvrgBuffer += wFreqBuffer;                 // Sum the whole periods FIFO
        PeriodMeasAux.bDirection = SensorPeriod[wIndex].bDirection;
    }
    // Round to upper value
    wAvrgBuffer = (uint32_t)(wAvrgBuffer + (HALL_SPEED_FIFO_SIZE / 2) - 1);
    wAvrgBuffer /= HALL_SPEED_FIFO_SIZE; // Average value

    PeriodMeasAux.wPeriod = wAvrgBuffer;

    return (PeriodMeasAux);
}

/**
 * @brief:速度0.1hz
 * @return {*} : PeriodMeas_s
 */
PeriodMeas_s GetLastHallPeriod(void)
{
    PeriodMeas_s PeriodMeasAux;
    uint8_t bLastSpeedFIFO_Index;

    // Store current index to prevent errors if Capture occurs during processing
    bLastSpeedFIFO_Index = bSpeedFIFO_Index;

    // This is done assuming interval between captures is higher than time
    // to read the two values
    PeriodMeasAux.wPeriod = SensorPeriod[bLastSpeedFIFO_Index].hCapture;
    PeriodMeasAux.wPeriod *= (SensorPeriod[bLastSpeedFIFO_Index].hPrscReg + 1);

    PeriodMeasAux.bDirection = SensorPeriod[bLastSpeedFIFO_Index].bDirection;
    return (PeriodMeasAux);
}

/* HALL的溢出/非溢出处理、计算dpp */
int16_t HALL_GetRotorFreq(void)
{
    PeriodMeas_s PeriodMeasAux;

    if (DoRollingAverage) {
        PeriodMeasAux = GetAvrgHallPeriod();
    } else { // Raw period
        PeriodMeasAux = GetLastHallPeriod();
    }

    if (HallTimeOut == TRUE) {
        hRotorFreq_dpp = 0;
    } else {
        if (PeriodMeasAux.bDirection != ERROR)
        // No errors have been detected during rotor speed information extrapolation
        {
            if (htim3.Instance->PSC >= HALL_MAX_RATIO) /* At start-up or very low freq */
            {                                          /* Based on current prescaler value only */
                hRotorFreq_dpp = 0;
            } else {
                if (PeriodMeasAux.wPeriod > MAX_PERIOD) /* Speed is too low */
                {
                    hRotorFreq_dpp = 0;
                } else { /*Avoid u32 DIV Overflow*/
                    if (PeriodMeasAux.wPeriod > (uint32_t)SPEED_OVERFLOW) {
                        if (HALL_GetCaptCounter() < 2) // First capture must be discarded
                        {
                            hRotorFreq_dpp = 0;
                        } else {
                            hRotorFreq_dpp = (int16_t)((uint16_t)(PSEUDO_FREQ_CONV /
                                                                  PeriodMeasAux.wPeriod));
                            hRotorFreq_dpp *= PeriodMeasAux.bDirection;
                        }
                    } else {
                        hRotorFreq_dpp = HALL_MAX_PSEUDO_SPEED;
                    }
                }
            }
        }
    }

    return (hRotorFreq_dpp);
}

/* TIM3 CC1 Capture Interrupt  */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    static uint8_t bHallState = 0;
    static uint8_t bPrevHallState = 0;
    uint32_t wCaptBuf = 0;
    uint16_t hPrscBuf;

    uint16_t hCaptBuf;
    int8_t bSpeedAux;
    uint32_t wIndex;

    if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) {
        bPrevHallState = bHallState;
        bHallState = HALL_U_VALUE << 2 | HALL_V_VALUE << 1 | HALL_W_VALUE << 0;

        switch (bHallState) {
        case STATE_5:
            if (bPrevHallState == STATE_5) {
                // a speed reversal occured
                if (bSpeed < 0) {
                    bSpeed = POSITIVE_SWAP;
                } else {
                    bSpeed = NEGATIVE_SWAP;
                }
            } else if (bPrevHallState == STATE_6) {
                bSpeed = POSITIVE;
            } else if (bPrevHallState == STATE_3) {
                bSpeed = NEGATIVE;
            }
            // Update angle
            if (bSpeed < 0) // 负方向
            {
                hElectrical_Angle = (int16_t)(S16_PHASE_SHIFT + S16_60_PHASE_SHIFT);
            } else if (bSpeed != ERROR) {
                hElectrical_Angle = S16_PHASE_SHIFT;
            }
            break;

        case STATE_3:
            if (bPrevHallState == STATE_3) {
                // a speed reversal occured
                if (bSpeed < 0) {
                    bSpeed = POSITIVE_SWAP;
                } else {
                    bSpeed = NEGATIVE_SWAP;
                }
            } else if (bPrevHallState == STATE_5) {
                bSpeed = POSITIVE;
            } else if (bPrevHallState == STATE_6) {
                bSpeed = NEGATIVE;
            }
            // Update of the electrical angle
            if (bSpeed < 0) {
                hElectrical_Angle = (int16_t)(S16_PHASE_SHIFT + S16_120_PHASE_SHIFT +
                                              S16_60_PHASE_SHIFT);
            } else if (bSpeed != ERROR) {
                hElectrical_Angle = (int16_t)(S16_PHASE_SHIFT + S16_120_PHASE_SHIFT);
            }
            break;

        case STATE_6:
            if (bPrevHallState == STATE_6) {
                if (bSpeed < 0) {
                    bSpeed = POSITIVE_SWAP;
                } else {
                    bSpeed = NEGATIVE_SWAP;
                }
            }

            if (bPrevHallState == STATE_3) {
                bSpeed = POSITIVE;
            } else if (bPrevHallState == STATE_5) {
                bSpeed = NEGATIVE;
            }
            if (bSpeed < 0) {
                hElectrical_Angle = (int16_t)(S16_PHASE_SHIFT - S16_60_PHASE_SHIFT);
            } else if (bSpeed != ERROR) {
                hElectrical_Angle = (int16_t)(S16_PHASE_SHIFT - S16_120_PHASE_SHIFT);
            }
            break;

        default:
            bSpeed = ERROR;
            break;
        }

        // used for discarding first capture
        if (hCaptCounter < U16_MAX) {
            hCaptCounter++;
        }

        // Compute new array index
        if (bSpeedFIFO_Index != HALL_SPEED_FIFO_SIZE - 1) {
            bSpeedFIFO_Index++;
        } else {
            bSpeedFIFO_Index = 0;
        }

        // Timeout Flag is cleared when receiving an IC
        HALL_ClrTimeOut();

        // Store the latest speed acquisition
        if (bGP1_OVF_Counter != 0) // There was counter overflow before capture
        {
            wCaptBuf = (uint32_t)__HAL_TIM_GET_COUNTER(&htim3);

            hPrscBuf = htim3.Instance->PSC;

            while (bGP1_OVF_Counter != 0) // 溢出
            {
                wCaptBuf += 0x10000uL; // Compute the real captured value (> 16-bit)
                bGP1_OVF_Counter--;
                // OVF Counter is 8-bit and Capt is 16-bit, thus max CaptBuf is 24-bits
            }
            while (wCaptBuf > U16_MAX) {
                wCaptBuf /= 2; // Make it fit 16-bit using virtual prescaler
                // Reduced resolution not a problem since result just slightly < 16-bit
                hPrscBuf = (hPrscBuf * 2) + 1;
                if (hPrscBuf > U16_MAX / 2) // Avoid Prsc overflow
                {
                    hPrscBuf = U16_MAX;
                    wCaptBuf = U16_MAX;
                }
            }
            SensorPeriod[bSpeedFIFO_Index].hCapture = wCaptBuf;
            SensorPeriod[bSpeedFIFO_Index].hPrscReg = hPrscBuf;
            SensorPeriod[bSpeedFIFO_Index].bDirection = bSpeed;
            if (RatioInc) //
            {
                RatioInc = FALSE; // Previous capture caused overflow
                                  // Don't change prescaler (delay due to preload/update mechanism)
            } else {
                if ((htim3.Instance->PSC) < HALL_MAX_RATIO) // Avoid OVF w/ very low freq
                {
                    (htim3.Instance->PSC)++; // To avoid OVF during speed decrease
                    RatioInc = TRUE;         // new prsc value updated at next capture only
                }
            }
        } else // No counter overflow
        {
            uint16_t hHighSpeedCapture, hClockPrescaler;

            hHighSpeedCapture = (uint32_t)__HAL_TIM_GET_COUNTER(&htim3);

            SensorPeriod[bSpeedFIFO_Index].hCapture = hHighSpeedCapture;
            SensorPeriod[bSpeedFIFO_Index].bDirection = bSpeed;
            // Store prescaler directly or incremented if value changed on last capt
            hClockPrescaler = htim3.Instance->PSC;

            // If prsc preload reduced in last capture, store current register + 1
            if (RatioDec) // and don't decrease it again
            {
                SensorPeriod[bSpeedFIFO_Index].hPrscReg = (hClockPrescaler) + 1;
                RatioDec = FALSE;
            } else // If prescaler was not modified on previous capture
            {
                if (hHighSpeedCapture >= LOW_RES_THRESHOLD) {
                    SensorPeriod[bSpeedFIFO_Index].hPrscReg = hClockPrescaler;
                } else {
                    if (htim3.Instance->PSC) // or prescaler cannot be further reduced
                    {
                        SensorPeriod[bSpeedFIFO_Index].hPrscReg = hClockPrescaler;
                    } else // The prescaler needs to be modified to optimize the accuracy
                    {
                        SensorPeriod[bSpeedFIFO_Index].hPrscReg = hClockPrescaler;
                        (htim3.Instance->PSC)--; // Increase accuracy by decreasing prsc
                        // Avoid decrementing again in next capt.(register preload delay)
                        RatioDec = TRUE;
                    }
                }
            }
        }

        if (InitRollingAverage) // 初始化滑动均值滤波
        {

            // Read last captured value and copy it into the whole array
            hCaptBuf = SensorPeriod[bSpeedFIFO_Index].hCapture;
            hPrscBuf = SensorPeriod[bSpeedFIFO_Index].hPrscReg;
            bSpeedAux = SensorPeriod[bSpeedFIFO_Index].bDirection;

            for (wIndex = 0; wIndex != HALL_SPEED_FIFO_SIZE - 1; wIndex++) {
                SensorPeriod[wIndex].hCapture = hCaptBuf;
                SensorPeriod[wIndex].hPrscReg = hPrscBuf;
                SensorPeriod[wIndex].bDirection = bSpeedAux;
            }
            InitRollingAverage = FALSE;
            // Starting from now, the values returned by MTC_GetRotorFreq are averaged
            DoRollingAverage = TRUE; // 开启滑动均值滤波算dpp
        }

        // Update Rotor Frequency Computation
    }
    hRotorFreq_dpp = HALL_GetRotorFreq();
}
