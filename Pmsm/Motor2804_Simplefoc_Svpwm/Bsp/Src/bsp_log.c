#include <stdio.h>

#define LOG_TAG    "LOGGER"
#include "bsp_log.h"


void Log_Init(void)
{
    /* initialize EasyLogger */
    elog_init();
    /* set EasyLogger log format */
    elog_set_fmt(ELOG_LVL_ASSERT, ELOG_FMT_ALL);
    elog_set_fmt(ELOG_LVL_ERROR, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
    elog_set_fmt(ELOG_LVL_WARN, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
    elog_set_fmt(ELOG_LVL_INFO, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
    elog_set_fmt(ELOG_LVL_DEBUG, ELOG_FMT_ALL & ~(ELOG_FMT_FUNC | ELOG_FMT_T_INFO | ELOG_FMT_P_INFO));
    elog_set_fmt(ELOG_LVL_VERBOSE, ELOG_FMT_ALL & ~(ELOG_FMT_FUNC | ELOG_FMT_T_INFO | ELOG_FMT_P_INFO));

    elog_set_filter_lvl(ELOG_LVL_INFO);
    elog_start();

    LOG_I("LOG Init Success\r\n");
}

void Log_Test(void)
{
    /* test log output for all level */
    LOG_A("Hello EasyLogger!\r\n");
    LOG_E("Hello EasyLogger!\r\n");
    LOG_W("Hello EasyLogger!\r\n");
    LOG_I("Hello EasyLogger!\r\n");
    LOG_D("Hello EasyLogger!\r\n");
    LOG_V("Hello EasyLogger!\r\n");
}




