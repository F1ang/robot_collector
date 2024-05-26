#include "lv_mainstart.h"
#include "lvgl.h"
#include <stdio.h>
#include "led.h"

///**
// * @brief  LVGL演示
// * @param  无
// * @return 无
// */
//void lv_mainstart(void)
//{
//    lv_example_label();                                           /* 加载提示标签 */
//    lv_example_bar();                                             /* 加载进度条 */
//}


/* 文件系统Fs */

/**
* @brief 获取指针位置
* @param fd：文件指针
* @return 返回名称
*/
long lv_tell(lv_fs_file_t *fd)
{
	uint32_t pos = 0;
	lv_fs_tell(fd, &pos);
	return pos;
}
/**
* @brief 文件系统测试
* @param 无
* @return 无
*/
static void lv_fs_test(void)
{
	char rbuf[30] = {0};
	uint32_t rsize = 0;
	lv_fs_file_t fd;
	lv_fs_res_t res;
	res = lv_fs_open(&fd, "0:/SYSTEM/LV_FATFS/Fatfs_test.txt", LV_FS_MODE_RD);
	if (res != LV_FS_RES_OK)
	{
		printf("open 0:/Fatfs_test.txt ERROR\n");
		return ;
	}
	lv_tell(&fd);
	lv_fs_seek(&fd, 0,LV_FS_SEEK_SET);
	lv_tell(&fd);
	res = lv_fs_read(&fd, rbuf, 100, &rsize);
	if (res != LV_FS_RES_OK)
	{
		printf("read ERROR\n");
		return ;
	}
	lv_tell(&fd);
	printf("READ(%d): %s",rsize , rbuf);
	lv_fs_close(&fd);
}


/**
* @brief 文件系统演示
* @param 无
* @return 无
*/
//void lv_mainstart(void)
//{
//	lv_obj_t *label = lv_label_create(lv_scr_act());
//	lv_label_set_text(label, "FATFS TEST");
//	lv_obj_center(label);
//	lv_fs_test();
//}

/* 字库 */
// void lv_mainstart(void)
// {
// 	lv_obj_t *label = lv_label_create(lv_scr_act());
// 	lv_label_set_text(label, LV_SYMBOL_AUDIO"AUDIO");
// }



// /* 对象 */
// static lv_coord_t scr_act_width, scr_act_height;    /* 活动屏幕宽度、高度 */
// static lv_obj_t *parent_obj;                        /* 定义父对象 */
// static lv_obj_t *child_obj;                         /* 定义子对象 */

// /**
//  * @brief  基础对象事件回调
//  * @param  *e ：事件相关参数的集合，它包含了该事件的所有数据
//  * @return 无
//  */

// static void obj_event_cb(lv_event_t *e)
// {
//     lv_obj_t * target = lv_event_get_target(e);                                 /* 获取事件触发源 */

//     if (target == parent_obj)                                                   /* 判断触发源：是不是父对象？ */
//     {
//         lv_obj_align(parent_obj, LV_ALIGN_CENTER, 0, 0);                        /* 重新设置父对象的位置：居中 */
//     }
//     else if (target == child_obj)                                               /* 判断触发源：是不是子对象？ */
//     {
//         lv_obj_align(child_obj, LV_ALIGN_RIGHT_MID, 100, 0);                    /* 重新设置子对象的位置：右侧居中，再向X轴偏移100 */
//     }
// }

// void lv_mainstart(void)
// {
//     /* 动态获取屏幕大小 */
//     scr_act_width = lv_obj_get_width(lv_scr_act());
//     scr_act_height = lv_obj_get_height(lv_scr_act());

//     /* 父对象 */
//     parent_obj = lv_obj_create(lv_scr_act());
//     lv_obj_set_size(parent_obj, scr_act_width, scr_act_height);
//     lv_obj_align(parent_obj, LV_ALIGN_CENTER, 0, 0);
//     lv_obj_set_style_bg_color(parent_obj, lv_color_hex(0x99ccff), 0);           /* 设置父对象的背景色：浅蓝色 */
//     lv_obj_add_event_cb(parent_obj, obj_event_cb, LV_EVENT_LONG_PRESSED, NULL); /* 为父对象添加事件：长按触发 */

//     /* 子对象 */
//     child_obj = lv_obj_create(parent_obj);                                      /* 创建子对象 */
//     lv_obj_set_size(child_obj, scr_act_width / 3, scr_act_height / 3);          /* 设置子对象的大小 */
//     lv_obj_align(child_obj, LV_ALIGN_CENTER, 0, 0);                             /* 设置子对象的位置：居中 */
//     lv_obj_set_style_bg_color(child_obj, lv_color_hex(0x003366), 0);            /* 设置子对象的背景色：深蓝色 */
//     lv_obj_add_event_cb(child_obj, obj_event_cb, LV_EVENT_CLICKED, NULL);       /* 为子对象添加事件：按下释放后触发 */
// }


/*------------- 进度条 -------------*/

// /* 获取当前活动屏幕的宽高 */
// #define scr_act_width() lv_obj_get_width(lv_scr_act())
// #define scr_act_height() lv_obj_get_height(lv_scr_act())

// static int16_t val = 0;                         /* 当前值 */
// static const lv_font_t *font;                   /* 定义字体 */

// static lv_obj_t *label_load;                    /* 加载标题标签 */
// static lv_obj_t *label_per;                     /* 百分比标签 */
// static lv_obj_t *bar;                           /* 进度条 */

// static void timer_cb(lv_timer_t *timer)
// {
//     if(val < 100)                                                           /* 当前值小于100 */
//     {
//         val ++;
//         lv_bar_set_value(bar, val, LV_ANIM_ON);                             /* 设置当前值 */
//         lv_label_set_text_fmt(label_per, "%d %%", lv_bar_get_value(bar));   /* 获取当前值，更新显示 */
//     }
//     else                                                                    /* 当前值大于等于100 */
//     {
//         lv_label_set_text(label_per, "finished!");                          /* 加载完成 */
//     }
// }

// static void lv_example_label(void)
// {
//         /* 根据活动屏幕宽度选择字体 */
//     if (scr_act_width() <= 480)
//     {
//         font = &lv_font_montserrat_14;
//     }
//     else
//     {
//         font = &lv_font_montserrat_20;
//     }

//     /* 加载标题标签 */
//     label_load = lv_label_create(lv_scr_act());
//     lv_label_set_text(label_load, "LOADING...");
//     lv_obj_set_style_text_font(label_load, font, LV_STATE_DEFAULT);
//     lv_obj_align(label_load, LV_ALIGN_CENTER, 0, -scr_act_height() / 10 );

//     /* 百分比标签 */
//     label_per = lv_label_create(lv_scr_act());
//     lv_label_set_text(label_per, "%0");
//     lv_obj_set_style_text_font(label_per, font, LV_STATE_DEFAULT);
//     lv_obj_align(label_per, LV_ALIGN_CENTER, 0, scr_act_height() / 10 );
// }

// static void lv_example_bar(void)
// {
//     bar = lv_bar_create(lv_scr_act());                            /* 创建进度条 */
//     lv_obj_set_align(bar, LV_ALIGN_CENTER);                       /* 设置位置 */
//     lv_obj_set_size(bar, scr_act_width() * 3 / 5, 20);            /* 设置大小 */
//     lv_obj_set_style_anim_time(bar, 100, LV_STATE_DEFAULT);       /* 设置动画时间 */
//     lv_timer_create(timer_cb, 100, NULL);                         /* 初始化定时器 */
// }

// void lv_mainstart(void)
// {
//     lv_example_label();                                           /* 加载提示标签 */
//     lv_example_bar();                                             /* 加载进度条 */
// }

/*------------- 进度条 -------------*/

/*------------- 按钮 -------------*/
// static const lv_font_t *font;
// static  lv_obj_t *label_speed;
// static  lv_obj_t *btn_speedup;
// static  lv_obj_t *btn_speeddown;
// static  lv_obj_t *btn_stop;

// static  int32_t speed_val = 0;

// #define scr_act_width() lv_obj_get_width(lv_scr_act())
// #define scr_act_height()  lv_obj_get_height(lv_scr_act())

// static void lv_example_label(void)
// {
//     if (scr_act_width() <= 320)
//     {
//         font = &lv_font_montserrat_14;
//     }
//     else if (scr_act_width() <= 480)
//     {
//         font = &lv_font_montserrat_14;
//     }
//     else
//     {
//         font = &lv_font_montserrat_20;
//     }

//     label_speed = lv_label_create(lv_scr_act());
//     lv_obj_set_style_text_font(label_speed, font, LV_PART_MAIN);
//     lv_label_set_text(label_speed, "Speed:");
//     lv_obj_align(label_speed, LV_ALIGN_CENTER, 0, -scr_act_height()/3);
// }

// static void btn_event_cb(lv_event_t * e)
// {
//     lv_obj_t *target = lv_event_get_target(e);      /* 获取触发源 */

//     if(target == btn_speedup)                      /* 加速按钮 */
//     {
//         speed_val += 30;
//     }
//     else if(target == btn_speeddown)               /* 减速按钮 */
//     {
//         speed_val -= 30;
//     }
//     else if(target == btn_stop)                     /* 急停按钮 */
//     {
//         speed_val = 0;
//     }
//     lv_label_set_text_fmt(label_speed, "Speed : %d RPM", speed_val);    /* 更新速度值 */
// }

// static void lv_example_btn_up(void)
// {
//     btn_speedup = lv_btn_create(lv_scr_act());                                     /* 创建加速按钮 */
//     lv_obj_set_size(btn_speedup, scr_act_width() / 4, scr_act_height() / 6);       /* 设置按钮大小 */
//     lv_obj_align(btn_speedup, LV_ALIGN_CENTER, -scr_act_width() / 3, 0);           /* 设置按钮位置 */
//     lv_obj_add_event_cb(btn_speedup, btn_event_cb, LV_EVENT_CLICKED, NULL);        /* 设置按钮事件 */

//     lv_obj_t* label = lv_label_create(btn_speedup);                                /* 创建加速按钮标签--继承 */
//     lv_obj_set_style_text_font(label, font, LV_PART_MAIN);                          /* 设置字体 */
//     lv_label_set_text(label, "Speed +");                                            /* 设置标签文本 */
//     lv_obj_set_align(label,LV_ALIGN_CENTER);                                        /* 设置标签位置 */
// }

// static void lv_example_btn_down(void)
// {
//     btn_speeddown = lv_btn_create(lv_scr_act());                                   /* 创建加速按钮 */
//     lv_obj_set_size(btn_speeddown, scr_act_width() / 4, scr_act_height() / 6);     /* 设置按钮大小 */
//     lv_obj_align(btn_speeddown, LV_ALIGN_CENTER, 0, 0);                            /* 设置按钮位置 */
//     lv_obj_add_event_cb(btn_speeddown, btn_event_cb, LV_EVENT_CLICKED, NULL);      /* 设置按钮事件 */

//     lv_obj_t* label = lv_label_create(btn_speeddown);                              /* 创建减速按钮标签 */
//     lv_obj_set_style_text_font(label, font, LV_PART_MAIN);                          /* 设置字体 */
//     lv_label_set_text(label, "Speed -");                                            /* 设置标签文本 */
//     lv_obj_set_align(label,LV_ALIGN_CENTER);                                        /* 设置标签位置 */
// }

// static void lv_example_btn_stop(void)
// {
//     btn_stop = lv_btn_create(lv_scr_act());                                         /* 创建急停按钮 */
//     lv_obj_set_size(btn_stop, scr_act_width() / 4, scr_act_height() / 6);           /* 设置按钮大小 */
//     lv_obj_align(btn_stop, LV_ALIGN_CENTER, scr_act_width() / 3, 0);                /* 设置按钮位置 */
//     lv_obj_set_style_bg_color(btn_stop, lv_color_hex(0xef5f60), LV_STATE_DEFAULT);  /* 设置按钮背景颜色（默认） */
//     lv_obj_set_style_bg_color(btn_stop, lv_color_hex(0xff0000), LV_STATE_PRESSED);  /* 设置按钮背景颜色（按下） */
//     lv_obj_add_event_cb(btn_stop, btn_event_cb, LV_EVENT_CLICKED, NULL);            /* 设置按钮事件 */

//     lv_obj_t* label = lv_label_create(btn_stop);                                    /* 创建急停按钮标签 */
//     lv_obj_set_style_text_font(label, font, LV_PART_MAIN);                          /* 设置字体 */
//     lv_label_set_text(label, "Stop");                                               /* 设置标签文本 */
//     lv_obj_set_align(label,LV_ALIGN_CENTER);                                        /* 设置标签位置 */
// }

// void lv_mainstart(void)
// {
//     lv_example_label();             /* 速度提示标签 */
//     lv_example_btn_up();            /* 加速按钮 */
//     lv_example_btn_down();          /* 减速按钮 */
//     lv_example_btn_stop();          /* 急停按钮 */
// }
/*------------- 按钮 -------------*/

/*------------- 按钮矩阵 -------------*/
// /* 获取当前活动屏幕的宽高 */
// #define scr_act_width()  lv_obj_get_width(lv_scr_act())
// #define scr_act_height() lv_obj_get_height(lv_scr_act())

// /* 按钮矩阵数组 */
// static const char *num_map[] = { "1", "2", "3", "\n",
//                                  "4", "5", "6", "\n",
//                                  "7", "8", "9", "\n",
//                                  "#", "0", "%", "" };

// static uint16_t zoom_val;                           /* 图片缩放系数 */
// static lv_point_t points[] = {{0,0},{0,200}};       /* 线条坐标点数组 */

// static const lv_font_t *font;                       /* 定义字体 */
// static lv_obj_t *label_input;                       /* 输入框标签 */
// static lv_obj_t *obj_input;                         /* 输入框背景 */

// LV_IMG_DECLARE(img_user);                           /* 声明图片 */

// static void btnm_event_cb(lv_event_t* e)
// {
//     uint8_t id;
//     lv_event_code_t code = lv_event_get_code(e);                                /* 获取事件类型 */
//     lv_obj_t *target = lv_event_get_target(e);                                  /* 获取触发源 */

//     if (code == LV_EVENT_VALUE_CHANGED)
//     {
//         id = lv_btnmatrix_get_selected_btn(target);                             /* 获取按键索引 */
//         lv_label_set_text(label_input, lv_btnmatrix_get_btn_text(target, id));  /* 更新输入框标签文本 */
//         lv_obj_align_to(label_input, obj_input, LV_ALIGN_CENTER, 0, 0);         /* 设置标签位置 */
//     }
// }

// static void lv_example_btnmatrix(void)
// {

//     /* 根据屏幕宽度选择字体和图片缩放系数 */
//     if (scr_act_width() <= 480)
//     {
//         font = &lv_font_montserrat_14;
//         zoom_val = 128;
//     }
//     else
//     {
//         font = &lv_font_montserrat_16;
//         zoom_val = 256;
//     }

//     /* 图片显示 */
//     lv_obj_t *img = lv_img_create(lv_scr_act());                                                /* 创建图片部件 */
//     lv_img_set_src(img, &img_user);                                                             /* 设置图片源 */
//     lv_img_set_zoom(img, zoom_val);                                                             /* 设置图片缩放 */
//     lv_obj_align(img, LV_ALIGN_CENTER, -scr_act_width()/4, -scr_act_height()/7);                /* 设置位置 */
//     lv_obj_set_style_img_recolor(img, lv_color_hex(0xf2f2f2),0);                                /* 设置重新着色 */
//     lv_obj_set_style_img_recolor_opa(img,100,0);                                                /* 设置着色透明度 */

//     /* 用户标签 */
//     lv_obj_t *label_user = lv_label_create(lv_scr_act());                                       /* 创建标签 */
//     lv_label_set_text(label_user, "USER");                                                      /* 设置文本 */
//     lv_obj_set_style_text_font(label_user, font, LV_PART_MAIN);                                 /* 设置字体 */
//     lv_obj_set_style_text_align(label_user, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);                /* 设置文本居中 */
//     lv_obj_align_to(label_user, img, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);                           /* 设置位置 */

//     /* 输入框背景 */
//     obj_input = lv_obj_create(lv_scr_act());                                                    /* 创建基础对象 */
//     lv_obj_set_size(obj_input, scr_act_width()/4, scr_act_height()/12);                         /* 设置大小 */
//     lv_obj_align_to(obj_input, label_user, LV_ALIGN_OUT_BOTTOM_MID, 0, scr_act_height()/20);    /* 设置位置 */
//     lv_obj_set_style_bg_color(obj_input, lv_color_hex(0xcccccc),0);                             /* 设置背景颜色 */
//     lv_obj_set_style_bg_opa(obj_input, 150,0);                                                  /* 设置透明度 */
//     lv_obj_set_style_border_width(obj_input, 0 , 0);                                            /* 去除边框 */
//     lv_obj_set_style_radius(obj_input, 20, 0);                                                  /* 设置圆角 */
//     lv_obj_remove_style(obj_input, NULL, LV_PART_SCROLLBAR);                                    /* 移除滚动条 */

//     /* 输入框文本标签 */
//     label_input = lv_label_create(lv_scr_act());                                                /* 创建标签 */
//     lv_label_set_text(label_input, " ");                                                         /* 设置文本 */
//     lv_obj_set_style_text_font(label_input, font, LV_PART_MAIN);                                /* 设置字体 */
//     lv_obj_set_style_text_align(label_input, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);               /* 设置文本居中 */
//     lv_obj_align_to(label_input, obj_input, LV_ALIGN_CENTER, 0, 0);                             /* 设置位置 */

//     /* 分隔线 */
//     lv_obj_t *line = lv_line_create(lv_scr_act());                                              /* 创建线条 */
//     lv_line_set_points(line, points, 2);                                                        /* 设置线条坐标点 */
//     lv_obj_align(line, LV_ALIGN_CENTER, 0, 0);                                                  /* 设置位置 */
//     lv_obj_set_style_line_color(line, lv_color_hex(0xcdcdcd),0);                                /* 设置线条颜色 */

//     /* 按钮矩阵（创建） */
//     lv_obj_t *btnm = lv_btnmatrix_create(lv_scr_act());                                         /* 创建按钮矩阵 */
//     lv_obj_set_size(btnm, scr_act_width()* 2/5, scr_act_width()* 2/5);                          /* 设置大小 */
//     lv_btnmatrix_set_map(btnm, num_map);                                                        /* 设置按钮 */
//     lv_obj_align(btnm, LV_ALIGN_RIGHT_MID, -scr_act_width()/16, 0);                             /* 设置位置 */
//     lv_obj_set_style_text_font(btnm, font, LV_PART_ITEMS);                                      /* 设置字体 */

//     /* 按钮矩阵（优化界面） */
//     lv_obj_set_style_border_width(btnm, 0, LV_PART_MAIN);                                       /* 去除主体边框 */
//     lv_obj_set_style_bg_opa(btnm, 0, LV_PART_MAIN);                                             /* 设置主体背景透明度 */
//     lv_obj_set_style_bg_opa(btnm, 0, LV_PART_ITEMS);                                            /* 设置按钮背景透明度 */
//     lv_obj_set_style_shadow_width(btnm, 0, LV_PART_ITEMS);                                      /* 去除按钮阴影 */
//     lv_obj_add_event_cb(btnm, btnm_event_cb, LV_EVENT_VALUE_CHANGED, NULL);                     /* 设置按钮矩阵回调 */
// }

// void lv_mainstart(void)
// {
//     lv_example_btnmatrix();
// }

/*------------- 按钮矩阵 -------------*/

/*------------- 复选框 -------------*/
// /* 获取当前活动屏幕的宽高 */
// #define scr_act_width() lv_obj_get_width(lv_scr_act())
// #define scr_act_height() lv_obj_get_height(lv_scr_act())

// static int16_t aggregate = 0;                   /* 定义总价格 */

// static const lv_font_t *font;                   /* 定义字体 */

// static lv_obj_t *label_menu;                    /* 菜单标题标签 */
// static lv_obj_t *label_aggregate;               /* 总价格标签 */

// /* 定义复选框 */
// static lv_obj_t *checkbox1;
// static lv_obj_t *checkbox2;
// static lv_obj_t *checkbox3;
// static lv_obj_t *checkbox4;

// static void lv_example_label(void)
// {
//     /* 根据活动屏幕宽度选择字体 */
//     if (scr_act_width() <= 480)
//     {
//         font = &lv_font_montserrat_14;
//     }
//     else
//     {
//         font = &lv_font_montserrat_20;
//     }
    
//     /* 菜单标题标签 */
//     label_menu = lv_label_create(lv_scr_act());
//     lv_label_set_text(label_menu, "MENU");
//     lv_obj_set_style_text_font(label_menu, font, LV_STATE_DEFAULT);
//     lv_obj_align(label_menu, LV_ALIGN_CENTER, 0, -scr_act_height() * 2 / 5 );

//     /* 总价格标签 */
//     label_aggregate = lv_label_create(lv_scr_act());
//     lv_label_set_text(label_aggregate, "Aggregate  :  $0");
//     lv_obj_set_style_text_font(label_aggregate, font, LV_STATE_DEFAULT);
//     lv_obj_align(label_aggregate, LV_ALIGN_CENTER, 0, scr_act_height() * 2 / 5 );
// }

// static void checkbox_event_cb(lv_event_t *e)
// {
//     lv_obj_t *target = lv_event_get_target(e);                              /* 获取触发源 */

//     if(target == checkbox1)                                                 /* 复选框1触发 */
//     {
//         lv_obj_has_state(checkbox1, LV_STATE_CHECKED) ? (aggregate += 19) : (aggregate -= 19);
//     }
//     else if(target == checkbox2)                                            /* 复选框2触发 */
//     {
//         lv_obj_has_state(checkbox2, LV_STATE_CHECKED) ? (aggregate += 29) : (aggregate -= 29);
//     }

//     lv_label_set_text_fmt(label_aggregate, "Aggregate  :  $%d", aggregate); /* 更新总价格标签 */
// }

// static void lv_example_checkbox(void)
// {
//     /* 创建基础对象作为背景 */
//     lv_obj_t *obj = lv_obj_create(lv_scr_act());
//     lv_obj_set_size(obj, scr_act_width() * 4 /5 , scr_act_height() * 3 /5);
//     lv_obj_align(obj, LV_ALIGN_CENTER, 0 , 0);

//     /* 菜品1复选框 */
//     checkbox1 = lv_checkbox_create(obj);
//     lv_checkbox_set_text(checkbox1, "Roast chicken     $19");
//     lv_obj_set_style_text_font(checkbox1, font, LV_STATE_DEFAULT);
//     lv_obj_align(checkbox1, LV_ALIGN_LEFT_MID, 0, -scr_act_height() / 5 );
//     lv_obj_add_event_cb(checkbox1, checkbox_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

//     /* 菜品2复选框 */
//     checkbox2 = lv_checkbox_create(obj);
//     lv_checkbox_set_text(checkbox2, "Roast duck     $29");
//     lv_obj_set_style_text_font(checkbox2, font, LV_STATE_DEFAULT);
//     lv_obj_align_to(checkbox2, checkbox1, LV_ALIGN_OUT_BOTTOM_LEFT, 0, scr_act_height() / 16);
//     lv_obj_add_event_cb(checkbox2, checkbox_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

//     /* 菜品3复选框 */
//     checkbox3 = lv_checkbox_create(obj);
//     lv_checkbox_set_text(checkbox3, "Roast fish     $39");
//     lv_obj_set_style_text_font(checkbox3, font, LV_STATE_DEFAULT);
//     lv_obj_align_to(checkbox3, checkbox2, LV_ALIGN_OUT_BOTTOM_LEFT, 0, scr_act_height() / 16);
//     lv_obj_add_state(checkbox3, LV_STATE_DISABLED);

//     /* 菜品4复选框 */
//     checkbox4 = lv_checkbox_create(obj);
//     lv_checkbox_set_text(checkbox4, "Roast lamb     $69");
//     lv_obj_set_style_text_font(checkbox4, font, LV_STATE_DEFAULT);
//     lv_obj_align_to(checkbox4, checkbox3, LV_ALIGN_OUT_BOTTOM_LEFT, 0, scr_act_height() / 16);
//     lv_obj_add_state(checkbox4, LV_STATE_DISABLED);
// }

// void lv_mainstart(void)
// {
//     lv_example_label();     /* 菜单标题、总价标签 */
//     lv_example_checkbox();  /* 菜品复选框 */
// }
/*------------- 复选框 -------------*/

/*------------- 下拉 -------------*/
// /* 获取当前活动屏幕的宽高 */
// #define scr_act_width() lv_obj_get_width(lv_scr_act())
// #define scr_act_height() lv_obj_get_height(lv_scr_act())

// static lv_obj_t *label;                                             /* 当前选项标签 */
// static const lv_font_t *dropdown_font;                              /* 定义字体 */
// static int16_t dropdown_width;                                      /* 列表宽度 */

// /* 下拉列表的选项 */
// static const char *options =    "option 1\n"
//                                 "option 2\n"
//                                 "option 3\n"
//                                 "option 4\n"
//                                 "option 5\n"
//                                 "option 6\n"
//                                 "option 7\n"
//                                 "option 8\n";
// /* 下拉事件，触发此回调 */
// static void dropdown_event_cb(lv_event_t* e)
// {
//     lv_event_code_t code = lv_event_get_code(e);                    /* 获取事件类型 */
//     lv_obj_t *dropdown = lv_event_get_target(e);                    /* 获取触发源 */

//     if (LV_EVENT_VALUE_CHANGED == code)                             /* 判断事件类型 */
//     {
//         char buf[10];
//         lv_dropdown_get_selected_str(dropdown, buf, sizeof(buf));   /* 获取当前选项文本 */
//         lv_label_set_text(label, buf);                              /* 显示当前选项文本 */
//     }
// }

// static void lv_example_dropdown_1(void)
// {
//     /* 根据屏幕宽度选择字体和列表宽度 */
//     if (scr_act_width() <= 320) 
//     {
//         dropdown_font = &lv_font_montserrat_14;
//         dropdown_width = 90;
//     }
//     else if (scr_act_width() <= 480) 
//     {
//         dropdown_font = &lv_font_montserrat_12;
//         dropdown_width = 120;
//     }
//     else 
//     {
//         dropdown_font = &lv_font_montserrat_22;
//         dropdown_width = 150;
//     }

//     lv_obj_t* dropdown = lv_dropdown_create(lv_scr_act());                                      /* 定义并创建下拉列表 */
//     lv_dropdown_set_options_static(dropdown, options);                                          /* 添加下拉列表选项 */
//     lv_obj_set_style_text_font(dropdown, dropdown_font, LV_PART_MAIN);                          /* 设置下拉列表字体 */
//     lv_obj_set_width(dropdown, dropdown_width);                                                 /* 设置下拉列表宽度 */
//     lv_obj_align(dropdown, LV_ALIGN_CENTER, -scr_act_width() / 3, 0);                           /* 设置下拉列表位置 */

//     label = lv_label_create(lv_scr_act());                                                      /* 定义并创建标签 */
//     lv_obj_set_style_text_font(label, dropdown_font, LV_PART_MAIN);                             /* 设置标签表字体 */
//     lv_obj_set_width(label, dropdown_width);                                                    /* 设置标签宽度 */
//     lv_obj_align_to(label, dropdown, LV_ALIGN_OUT_TOP_MID, 15, -scr_act_height() / 8);          /* 设置标签位置 */
//     lv_label_set_text(label, "option 1");                                                       /* 设置标签文本 */

//     lv_obj_add_event_cb(dropdown, dropdown_event_cb, LV_EVENT_VALUE_CHANGED, NULL);             /* 添加下拉列表回调 */
// }

// /**
//  * @brief  例2
//  * @param  无
//  * @return 无
//  */
// static void lv_example_dropdown_2(void)
// {
//     lv_obj_t* dropdown;                                                                         /* 定义下拉列表 */
    
//     dropdown = lv_dropdown_create(lv_scr_act());                                                /* 创建下拉列表 */
//     lv_dropdown_set_options_static(dropdown, options);                                          /* 添加下拉列表选项 */
//     lv_obj_set_style_text_font(dropdown, dropdown_font, LV_PART_MAIN);                          /* 设置下拉列表字体 */
//     lv_obj_set_width(dropdown, dropdown_width);                                                 /* 设置下拉列表宽度 */
//     lv_dropdown_set_dir(dropdown, LV_DIR_BOTTOM);                                               /* 设置下拉列表方向 */
//     lv_dropdown_set_symbol(dropdown, LV_SYMBOL_DOWN);                                           /* 设置下拉列表符号 */
//     lv_obj_align(dropdown, LV_ALIGN_CENTER, scr_act_width() / 8, -3 * scr_act_height() / 8);    /* 设置下拉列表位置 */

//     dropdown = lv_dropdown_create(lv_scr_act());                                                /* 创建下拉列表 */
//     lv_dropdown_set_options_static(dropdown, options);                                          /* 添加下拉列表选项 */
//     lv_obj_set_style_text_font(dropdown, dropdown_font, LV_PART_MAIN);                          /* 设置下拉列表字体 */
//     lv_obj_set_width(dropdown, dropdown_width);                                                 /* 设置下拉列表宽度 */
//     lv_dropdown_set_dir(dropdown, LV_DIR_LEFT);                                                 /* 设置下拉列表方向 */
//     lv_dropdown_set_symbol(dropdown, LV_SYMBOL_LEFT);                                           /* 设置下拉列表符号 */
//     lv_obj_align(dropdown, LV_ALIGN_CENTER, scr_act_width() / 8, -1 * scr_act_height() / 8);    /* 设置下拉列表位置 */

//     dropdown = lv_dropdown_create(lv_scr_act());                                                /* 创建下拉列表 */
//     lv_dropdown_set_options_static(dropdown, options);                                          /* 添加下拉列表选项 */
//     lv_obj_set_style_text_font(dropdown, dropdown_font, LV_PART_MAIN);                          /* 设置下拉列表字体 */
//     lv_obj_set_width(dropdown, dropdown_width);                                                 /* 设置下拉列表宽度 */
//     lv_dropdown_set_dir(dropdown, LV_DIR_RIGHT);                                                /* 设置下拉列表方向 */
//     lv_dropdown_set_symbol(dropdown, LV_SYMBOL_RIGHT);                                          /* 设置下拉列表符号 */
//     lv_obj_align(dropdown, LV_ALIGN_CENTER, scr_act_width() / 8, 1 * scr_act_height() / 8);     /* 设置下拉列表位置 */

//     dropdown = lv_dropdown_create(lv_scr_act());                                                /* 创建下拉列表 */
//     lv_dropdown_set_options_static(dropdown, options);                                          /* 添加下拉列表选项 */
//     lv_obj_set_style_text_font(dropdown, dropdown_font, LV_PART_MAIN);                          /* 设置下拉列表字体 */
//     lv_obj_set_width(dropdown, dropdown_width);                                                 /* 设置下拉列表宽度 */
//     lv_dropdown_set_dir(dropdown, LV_DIR_TOP);                                                  /* 设置下拉列表方向 */
//     lv_dropdown_set_symbol(dropdown, LV_SYMBOL_UP);                                             /* 设置下拉列表符号 */
//     lv_obj_align(dropdown, LV_ALIGN_CENTER, scr_act_width() / 8, 3 * scr_act_height() / 8);     /* 设置下拉列表位置 */
// }

// void lv_mainstart(void)
// {
//     lv_example_dropdown_1();
//     lv_example_dropdown_2();
// }
/*------------- 下拉 -------------*/


/*------------- 图片 -------------*/
// /* 获取当前活动屏幕的宽高 */
// #define scr_act_width()  lv_obj_get_width(lv_scr_act())
// #define scr_act_height() lv_obj_get_height(lv_scr_act())

// /* 定义图片部件、滑块部件 */
// static lv_obj_t *img;
// static lv_obj_t *slider_zoom, *slider_angle, *slider_r, *slider_g, *slider_b, *slider_opa;

// LV_IMG_DECLARE(img_user);                     /* 声明图片 */

// /**
//  * @brief  滑块事件回调
//  * @param  *e ：事件相关参数的集合，它包含了该事件的所有数据
//  * @return 无
//  */
// static void slider_event_cb(lv_event_t *e)
// {
//     lv_img_set_zoom(img, lv_slider_get_value(slider_zoom));                                         /* 设置图片缩放 */
//     lv_img_set_angle(img, lv_slider_get_value(slider_angle));                                       /* 设置图片旋转角度 */

//     /* 设置图片重新着色 */
//     lv_obj_set_style_img_recolor(img,
//                                  lv_color_make(lv_slider_get_value(slider_r), lv_slider_get_value(slider_g), lv_slider_get_value(slider_b)),
//                                  LV_PART_MAIN);

//     lv_obj_set_style_img_recolor_opa(img, lv_slider_get_value(slider_opa), LV_PART_MAIN);           /* 设置重新着色透明度 */
// }

// /**
//  * @brief  创建滑块
//  * @param  color:颜色值
//  * @return *slider：创建成功的滑块部件
//  */
// static lv_obj_t *my_slider_create(lv_color_t color)
// {
//     lv_obj_t *slider = lv_slider_create(lv_scr_act());                                              /* 创建滑块 */
//     lv_obj_set_height(slider, scr_act_height() / 20);                                               /* 设置高度 */
//     lv_obj_set_width(slider, scr_act_width() / 3);                                                  /* 设置宽度 */
//     lv_obj_remove_style(slider, NULL, LV_PART_KNOB);                                                /* 移除旋钮 */
//     lv_obj_set_style_bg_color(slider,color,LV_PART_INDICATOR);                                      /* 设置滑块指示器颜色 */
//     lv_obj_set_style_bg_color(slider,lv_color_darken(color, 100),LV_PART_MAIN);                     /* 设置滑块主体颜色、透明度 */
//     lv_obj_add_event_cb(slider, slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);                     /* 设置滑块回调 */
//     return slider;
// }

// /**
//  * @brief  图片部件实例
//  * @param  无
//  * @return 无
//  */
// static void lv_example_img(void)
// {
//     img = lv_img_create(lv_scr_act());                                                              /* 创建图片部件 */
//     lv_img_set_src(img, &img_user);                                                                 /* 设置图片源 */
//     lv_obj_align(img, LV_ALIGN_CENTER, -scr_act_width() / 5, 0);                                    /* 设置图片位置 */
//     lv_obj_update_layout(img);                                                                      /* 更新图片参数 */

//     /* 图片缩放控制滑块 */
//     slider_zoom = my_slider_create(lv_color_hex(0x989c98));                                         /* 创建滑块 */
//     lv_slider_set_range(slider_zoom, 128, 512);                                                     /* 设置滑块的范围 */
//     lv_slider_set_value(slider_zoom, 256, LV_ANIM_OFF);                                             /* 设置滑块的值 */
//     lv_obj_align(slider_zoom, LV_ALIGN_CENTER, scr_act_width() / 4, -scr_act_height() / 4);         /* 设置滑块位置 */

//     /* 旋转角度控制滑块 */
//     slider_angle = my_slider_create(lv_color_hex(0x989c98));                                        /* 创建滑块 */
//     lv_slider_set_range(slider_angle, 0, 3600);                                                     /* 设置滑块的范围 */
//     lv_obj_align_to(slider_angle, slider_zoom, LV_ALIGN_OUT_BOTTOM_LEFT, 0, scr_act_height() / 20); /* 设置滑块位置 */

//     /* 红色通道控制滑块 */
//     slider_r = my_slider_create(lv_color_hex(0xff0000));                                            /* 创建滑块 */
//     lv_slider_set_range(slider_r, 0, 255);                                                          /* 设置滑块的范围 */
//     lv_obj_align_to(slider_r, slider_angle, LV_ALIGN_OUT_BOTTOM_LEFT, 0, scr_act_height() / 20);    /* 设置滑块位置 */

//     /* 绿色通道控制滑块 */
//     slider_g = my_slider_create(lv_color_hex(0x00ff00));                                            /* 创建滑块 */
//     lv_slider_set_range(slider_g, 0, 255);                                                          /* 设置滑块的范围 */
//     lv_obj_align_to(slider_g, slider_r, LV_ALIGN_OUT_BOTTOM_LEFT, 0, scr_act_height() / 20);        /* 设置滑块位置 */

//     /* 蓝色通道控制滑块 */
//     slider_b = my_slider_create(lv_color_hex(0x0000ff));                                            /* 创建滑块 */
//     lv_slider_set_range(slider_b, 0, 255);                                                          /* 设置滑块的范围 */
//     lv_obj_align_to(slider_b, slider_g, LV_ALIGN_OUT_BOTTOM_LEFT, 0, scr_act_height() / 20);        /* 设置滑块位置 */

//     /* 着色透明度控制滑块 */
//     slider_opa = my_slider_create(lv_color_hex(0x000000));                                          /* 创建滑块 */
//     lv_slider_set_range(slider_opa, 0, 255);                                                        /* 设置滑块的范围 */
//     lv_slider_set_value(slider_opa, 150, LV_ANIM_OFF);                                              /* 设置滑块的值 */
//     lv_obj_align_to(slider_opa, slider_b, LV_ALIGN_OUT_BOTTOM_LEFT, 0, scr_act_height() / 20);      /* 设置滑块位置 */
// }

// /**
//  * @brief  LVGL演示
//  * @param  无
//  * @return 无
//  */
// void lv_mainstart(void)
// {
//     lv_example_img();
// }

/*------------- 图片 -------------*/


/*------------- 标签 -------------*/
// /* 获取当前活动屏幕的宽高 */
// #define scr_act_width() lv_obj_get_width(lv_scr_act())
// #define scr_act_height() lv_obj_get_height(lv_scr_act())

// static const lv_font_t *font;

// /**
//  * @brief  例1
//  * @param  无
//  * @return 无
//  */
// static void lv_example_label_1(void)
// {
//     if (scr_act_width() <= 320)
//         font = &lv_font_montserrat_14;
//     else if (scr_act_width() <= 480)
//         font = &lv_font_montserrat_14;
//     else
//         font = &lv_font_montserrat_20;

//     lv_obj_t* label = lv_label_create(lv_scr_act());                                /* 定义并创建标签 */
//     lv_label_set_text(label, "#0000ff Re-color# #ff00ff words# #ff0000 of a# "      /* 设置标签文本 */
//                              "label, align the lines to the center"
//                              "and  wrap long text automatically.");
//     lv_label_set_recolor(label, true);                                              /* 启用标签文本重新着色 */
//     lv_obj_set_style_text_font(label, font, LV_PART_MAIN);                          /* 设置标签文本字体 */
//     lv_obj_set_width(label, scr_act_width() / 3);                                   /* 设置标签宽度 */
//     lv_obj_align(label, LV_ALIGN_CENTER, -scr_act_width() / 3, 0);                  /* 设置标签位置 */
//     lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);         /* 设置标签文本对齐方式 */
// }

// /**
//  * @brief  例2
//  * @param  无
//  * @return 无
//  */
// static void lv_example_label_2(void)
// {
//     lv_obj_t* label = lv_label_create(lv_scr_act());                                /* 定义并创建标签 */
//     lv_label_set_text(label, "It is a circularly scrolling text. ");                /* 设置标签文本 */
//     lv_obj_set_style_text_font(label, font, LV_PART_MAIN);                          /* 设置标签文本字体 */
//     lv_obj_set_width(label, scr_act_width() / 3);                                   /* 设置标签宽度 */
//     lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR);                   /* 设置标签长模式：循环滚动 */
//     lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);                                     /* 设置标签位置 */
// }

// /**
//  * @brief  例3
//  * @param  无
//  * @return 无
//  */
// static void lv_example_label_3(void)
// {
//     lv_obj_t* label = lv_label_create(lv_scr_act());                                /* 定义并创建标签 */
//     lv_label_set_text_fmt(label, "Label can set text like %s", "printf");           /* 设置标签文本 */
//     lv_obj_set_style_text_font(label, font, LV_PART_MAIN);                          /* 设置标签文本字体 */
//     lv_obj_set_width(label, scr_act_width() / 3);                                   /* 设置标签宽度 */
//     lv_obj_align(label, LV_ALIGN_CENTER, scr_act_width() / 3, 0);                   /* 设置标签位置 */
//     lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);         /* 设置标签文本对齐方式 */

//     lv_obj_t* label_shadow = lv_label_create(lv_scr_act());                         /* 定义并创建阴影标签 */
//     lv_label_set_text(label_shadow, lv_label_get_text(label));                      /* 设置标签文本 */
//     lv_obj_set_style_text_font(label_shadow, font, LV_PART_MAIN);                   /* 设置标签文本字体 */
//     lv_obj_set_width(label_shadow, scr_act_width() / 3);                            /* 设置标签宽度 */
//     lv_obj_set_style_text_opa(label_shadow, LV_OPA_30, LV_PART_MAIN);               /* 设置标签文本透明度 */
//     lv_obj_set_style_text_color(label_shadow, lv_color_black(), LV_PART_MAIN);      /* 设置标签文本颜色 */
//     lv_obj_set_style_text_align(label_shadow, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);  /* 设置标签文本对齐方式 */
//     lv_obj_align_to(label_shadow, label, LV_ALIGN_TOP_LEFT, 3, 3);                  /* 设置标签位置 */
// }

// /**
//  * @brief  LVGL演示
//  * @param  无
//  * @return 无
//  */
// void lv_mainstart(void)
// {
//     lv_example_label_1();
//     lv_example_label_2();
//     lv_example_label_3();
// }

/*------------- 标签 -------------*/


/*------------- 线条 -------------*/
// #define scr_act_width() lv_obj_get_width(lv_scr_act())          /* 获取当前活动屏幕的宽度 */
// #define scr_act_height() lv_obj_get_height(lv_scr_act())        /* 获取当前活动屏幕的高度 */

// #define SIN_POINTS_NUM   (200)                                  /* 正弦波坐标点数量 */

// static lv_point_t sin_line_points[SIN_POINTS_NUM];              /* 正弦波坐标数组 */
// static lv_point_t straigh_line_points[] = {{0,0},{100,0}};      /* 直线坐标数组 */
// static const lv_font_t *font;                                   /* 定义字体 */

// /**
//  * @brief       产生正弦波坐标点
//  * @param       maxval : 峰值
//  * @param       samples: 坐标点的个数
//  * @retval      无
//  */
// static void create_sin_buf(uint16_t maxval, uint16_t samples)
// {
//     uint16_t i;
//     float y = 0;

//     /*
//      * 正弦波最小正周期为2π，约等于2 * 3.1415926
//      * 曲线上相邻的两个点在x轴上的间隔 = 2 * 3.1415926 / 采样点数量
//      */
//     float inc = (2 * 3.1415926) / samples;                      /* 计算相邻两个点的x轴间隔 */

//     for (i = 0; i < samples; i++)                               /* 连续产生samples个点 */
//     {
//         /*
//          * 正弦波函数解析式：y = Asin(wx + φ）+ b
//          * 计算每个点的y值，将峰值放大maxval倍，并将曲线向上偏移maxval到正数区域
//          */
//         y = maxval * sin(inc * i) + maxval;

//         sin_line_points[i].x = 2 * i;                           /* 存入x轴坐标 */
//         sin_line_points[i].y = y;                               /* 存入y轴坐标 */
//     }
// }

// /**
//  * @brief  正弦波实例
//  * @param  无
//  * @return 无
//  */
// static void lv_example_line(void)
// {
//     /* 根据活动屏幕宽度选择字体 */
//     if (scr_act_width() <= 480)
//     {
//         font = &lv_font_montserrat_14;
//     }
//     else
//     {
//         font = &lv_font_montserrat_14;
//     }

//     /* 标题 */
//     lv_obj_t *label = lv_label_create(lv_scr_act());                                        /* 创建标签 */
//     lv_label_set_text(label, "Line");                                                       /* 设置文本内容 */
//     lv_obj_set_style_text_font(label, font, LV_STATE_DEFAULT);                              /* 设置字体 */
//     lv_obj_align(label, LV_ALIGN_TOP_LEFT, scr_act_width() / 20, scr_act_height()/16);      /* 设置位置 */

//     /* 直线（用作下划线）*/
//     lv_obj_t *straigh_line = lv_line_create(lv_scr_act());                                  /* 创建线条 */
//     lv_line_set_points(straigh_line, straigh_line_points, 2);                               /* 设置线条坐标点 */
//     lv_obj_align_to(straigh_line, label, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0);                   /* 设置位置 */

//     // /* 正弦波 */
//     // lv_obj_t *sin_line = lv_line_create(lv_scr_act());                                      /* 创建线条 */
//     // create_sin_buf(scr_act_height()/4, SIN_POINTS_NUM);                                     /* 产生正弦波坐标点 */
//     // lv_line_set_points(sin_line, sin_line_points, SIN_POINTS_NUM);                          /* 设置线条坐标点 */
//     // lv_obj_center(sin_line);                                                                /* 设置位置 */
//     // lv_obj_set_style_line_width(sin_line, 8, LV_PART_MAIN);                                 /* 设置线的宽度 */
//     // lv_obj_set_style_line_color(sin_line, lv_palette_main(LV_PALETTE_BLUE), LV_PART_MAIN);  /* 设置线的颜色 */
//     // lv_obj_set_style_line_rounded(sin_line, true, LV_PART_MAIN);                            /* 设置线条圆角 */
// }

// /**
//  * @brief  LVGL演示
//  * @param  无
//  * @return 无
//  */
// void lv_mainstart(void)
// {
//     lv_example_line();         /* 正弦波实例 */
// }

/*------------- 滚轮 -------------*/
// /* 获取当前活动屏幕的宽高 */
// #define scr_act_width() lv_obj_get_width(lv_scr_act())
// #define scr_act_height() lv_obj_get_height(lv_scr_act())

// /* 模式选项 */
// static const char *mode_options = "Auto\n"
//                                     "Heat\n"
//                                     "Cool\n"
//                                     "Fan";
// /* 温度选项 */
// static const char *temp_options = "23\n"
//                                     "24\n"
//                                     "25\n"
//                                     "26\n"
//                                     "27\n"
//                                     "28\n"
//                                     "29";
// /* 湿度选项 */
// static const char *hum_options  = "30\n"
//                                     "40\n"
//                                     "50\n"
//                                     "60\n"
//                                     "70\n"
//                                     "80\n"
//                                     "90";

// static const lv_font_t *font;                   /* 定义字体 */
// static lv_obj_t *temp_roller;                   /* 温度滚轮 */
// static lv_obj_t *hum_roller;                    /* 湿度滚轮 */

// /**
//  * @brief  滚轮事件回调
//  * @param  *e ：事件相关参数的集合，它包含了该事件的所有数据
//  * @return 无
//  */
// static void roller_event_cb(lv_event_t* e)
// {
//     lv_obj_t *target = lv_event_get_target(e);                  /* 获取触发源 */

//     if(lv_roller_get_selected(target) == 0)                     /* 获取索引，判断是否为Auto选项 */
//     {
//         lv_obj_add_state(temp_roller, LV_STATE_DISABLED);       /* 设置温度滚轮为不可选状态 */
//         lv_obj_add_state(hum_roller, LV_STATE_DISABLED);        /* 设置湿度滚轮为不可选状态 */
//     }
//     else
//     {
//         lv_obj_clear_state(temp_roller, LV_STATE_DISABLED);     /* 解除温度滚轮不可选状态 */
//         lv_obj_clear_state(hum_roller, LV_STATE_DISABLED);      /* 解除湿度滚轮不可选状态 */
// 	}
// }

// /**
//  * @brief  模式设置
//  * @param  无
//  * @return 无
//  */
// static void lv_example_roller1(void)
// {
//     /* 根据活动屏幕宽度选择字体 */
//     if (scr_act_width() <= 480)
//     {
//         font = &lv_font_montserrat_14;
//     }
//     else
//     {
//         font = &lv_font_montserrat_20;
//     }

//     lv_obj_t *mode_roller = lv_roller_create(lv_scr_act());                                     /* 创建模式设置滚轮 */
//     lv_roller_set_options(mode_roller, mode_options, LV_ROLLER_MODE_NORMAL);                    /* 滚轮添加选项、设置正常模式 */
//     lv_obj_align(mode_roller, LV_ALIGN_CENTER, -scr_act_width() / 4, 0);                        /* 设置滚轮位置 */
//     lv_obj_set_width(mode_roller, scr_act_width() / 6);                                         /* 设置滚轮宽度 */
//     lv_obj_set_style_text_font(mode_roller, font, LV_STATE_DEFAULT);                            /* 设置滚轮字体 */
//     lv_roller_set_visible_row_count(mode_roller, 3);                                            /* 设置滚轮可见选项个数 */
//     lv_roller_set_selected(mode_roller, 2, LV_ANIM_OFF);                                        /* 设置滚轮当前所选项 */
//     lv_obj_add_event_cb(mode_roller, roller_event_cb, LV_EVENT_VALUE_CHANGED, NULL);            /* 添加事件回调 */

//     lv_obj_t *label = lv_label_create(lv_scr_act());                                            /* 创建标签 */
//     lv_label_set_text(label, "MODE");                                                           /* 设置文本内容 */
//     lv_obj_set_style_text_font(label, font, LV_STATE_DEFAULT);                                  /* 设置字体 */
//     lv_obj_align_to(label, mode_roller, LV_ALIGN_OUT_TOP_MID, 0, -15 );                         /* 设置位置 */
// }

// /**
//  * @brief  温度设置
//  * @param  无
//  * @return 无
//  */
// static void lv_example_roller2(void)
// {
//     temp_roller = lv_roller_create(lv_scr_act());                                               /* 创建温度设置滚轮 */
//     lv_roller_set_options(temp_roller, temp_options, LV_ROLLER_MODE_NORMAL);                    /* 滚轮添加选项、设置正常模式 */
//     lv_obj_align(temp_roller, LV_ALIGN_CENTER, 0, 0);                                           /* 设置滚轮位置 */
//     lv_obj_set_width(temp_roller, scr_act_width() / 6);                                         /* 设置滚轮宽度 */
//     lv_obj_set_style_text_font(temp_roller, font, LV_STATE_DEFAULT);                            /* 设置滚轮字体 */
//     lv_roller_set_visible_row_count(temp_roller, 3);                                            /* 设置滚轮可见选项个数 */
//     lv_roller_set_selected(temp_roller, 2, LV_ANIM_OFF);                                        /* 设置滚轮当前所选项 */

//     lv_obj_t *label = lv_label_create(lv_scr_act());                                            /* 创建标签 */
//     lv_label_set_text(label, "TEMP");                                                           /* 设置文本内容 */
//     lv_obj_set_style_text_font(label, font, LV_STATE_DEFAULT);                                  /* 设置字体 */
//     lv_obj_align_to(label, temp_roller, LV_ALIGN_OUT_TOP_MID, 0, -15 );                         /* 设置位置 */
// }

// /**
//  * @brief  湿度设置
//  * @param  无
//  * @return 无
//  */
// static void lv_example_roller3(void)
// {
//     hum_roller = lv_roller_create(lv_scr_act());                                                /* 创建湿度设置滚轮 */
//     lv_roller_set_options(hum_roller, hum_options, LV_ROLLER_MODE_NORMAL);                      /* 滚轮添加选项、设置正常模式 */
//     lv_obj_align(hum_roller, LV_ALIGN_CENTER, scr_act_width() / 4, 0);                          /* 设置滚轮位置 */
//     lv_obj_set_width(hum_roller, scr_act_width() / 6);                                          /* 设置滚轮宽度 */
//     lv_obj_set_style_text_font(hum_roller, font, LV_STATE_DEFAULT);                             /* 设置滚轮字体 */
//     lv_roller_set_visible_row_count(hum_roller, 3);                                             /* 设置滚轮可见选项个数 */
//     lv_roller_set_selected(hum_roller, 2, LV_ANIM_OFF);                                         /* 设置滚轮当前所选项 */

//     lv_obj_t *label = lv_label_create(lv_scr_act());                                            /* 创建标签 */
//     lv_label_set_text(label, "HUM");                                                            /* 设置文本内容 */
//     lv_obj_set_style_text_font(label, font, LV_STATE_DEFAULT);                                  /* 设置字体 */
//     lv_obj_align_to(label, hum_roller, LV_ALIGN_OUT_TOP_MID, 0, -15 );                          /* 设置位置 */
// }

// /**
//  * @brief  LVGL演示
//  * @param  无
//  * @return 无
//  */
// void lv_mainstart(void)
// {
//     lv_example_roller1();               /* 模式设置滚轮 */
//     lv_example_roller2();               /* 温度设置滚轮 */
//     lv_example_roller3();               /* 湿度设置滚轮 */
// }
/*------------- 滚轮 -------------*/

/*------------- 滑块 -------------*/
// /* 获取当前活动屏幕的宽高 */
// #define scr_act_width() lv_obj_get_width(lv_scr_act())
// #define scr_act_height() lv_obj_get_height(lv_scr_act())

// static const lv_font_t *font;                                                       /* 定义字体 */
// static lv_obj_t *slider_label;                                                      /* 进度条百分比标签 */

// /**
//  * @brief  滑块事件回调
//  * @param  *e ：事件相关参数的集合，它包含了该事件的所有数据
//  * @return 无
//  */
// static void slider_event_cb(lv_event_t *e)
// {
//     lv_obj_t *target = lv_event_get_target(e);                                      /* 获取触发源 */
//     lv_event_code_t code = lv_event_get_code(e);                                    /* 获取事件类型 */

//     if(code == LV_EVENT_VALUE_CHANGED)
//     {
//         lv_label_set_text_fmt(slider_label, "%d%%", lv_slider_get_value(target));   /* 获取当前值，更新进度条百分比 */
//     }
// }

// /**
//  * @brief  音量调节滑块
//  * @param  无
//  * @return 无
//  */
// static void lv_example_slider(void)
// {
//     /* 根据活动屏幕宽度选择字体 */
//     if (scr_act_width() <= 480)
//     {
//         font = &lv_font_montserrat_14;
//     }
//     else
//     {
//         font = &lv_font_montserrat_20;
//     }

//     /* 滑块 */
//     lv_obj_t * slider = lv_slider_create(lv_scr_act());                             /* 创建滑块 */
//     lv_obj_set_size(slider, scr_act_width() / 2, 20);                               /* 设置大小 */
//     lv_obj_center(slider);                                                          /* 设置位置 */
//     lv_slider_set_value(slider, 50, LV_ANIM_OFF);                                   /* 设置当前值 */
//     lv_obj_add_event_cb(slider, slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);     /* 添加事件 */

//     /* 百分比标签 */
//     slider_label = lv_label_create(lv_scr_act());                                   /* 创建百分比标签 */
//     lv_label_set_text(slider_label, "50%");                                         /* 设置文本内容 */
//     lv_obj_set_style_text_font(slider_label, font, LV_STATE_DEFAULT);               /* 设置字体 */
//     lv_obj_align_to(slider_label, slider, LV_ALIGN_OUT_RIGHT_MID, 20, 0);           /* 设置位置 */

//     /* 音量图标 */
//     lv_obj_t *sound_label = lv_label_create(lv_scr_act());                          /* 创建音量标签 */
//     lv_label_set_text(sound_label, LV_SYMBOL_VOLUME_MAX);                           /* 设置文本内容：音量图标 */
//     lv_obj_set_style_text_font(sound_label, font, LV_STATE_DEFAULT);                /* 设置字体 */
//     lv_obj_align_to(sound_label, slider, LV_ALIGN_OUT_LEFT_MID, -20, 0);            /* 设置位置 */
// }

// /**
//  * @brief  LVGL演示
//  * @param  无
//  * @return 无
//  */
// void lv_mainstart(void)
// {
//     lv_example_slider();
// }

/*------------- 滑块 -------------*/


/*------------- 开关 -------------*/
// /* 获取当前活动屏幕的宽高 */
// #define scr_act_width() lv_obj_get_width(lv_scr_act())
// #define scr_act_height() lv_obj_get_height(lv_scr_act())

// static lv_obj_t *switch_cool;            /* 制冷模式开关 */
// static lv_obj_t *switch_heat;            /* 制暖模式开关 */
// static lv_obj_t *switch_dry;             /* 干燥模式开关 */
// static const lv_font_t *font;            /* 定义字体 */

// /**
//  * @brief  回调事件
//  * @param  *e ：事件相关参数的集合，它包含了该事件的所有数据
//  * @return 无
//  */
// static void switch_event_cb(lv_event_t *e)
// {
//     lv_obj_t *target = lv_event_get_target(e);                      /* 获取触发源 */

//     if(target == switch_cool)                                       /* 制冷开关触发 */
//     {
//         if(lv_obj_has_state(switch_cool, LV_STATE_CHECKED))         /* 判断开关状态 */
//         {
//             lv_obj_clear_state(switch_heat, LV_STATE_CHECKED);      /* 制冷模式已打开，关闭制暖模式 */
//         }
//     }
//     else if(target == switch_heat)                                  /* 制暖开关触发 */
//     {
//         if(lv_obj_has_state(switch_heat, LV_STATE_CHECKED))         /* 判断开关状态 */
//         {
//             lv_obj_clear_state(switch_cool, LV_STATE_CHECKED);      /* 制暖模式已打开，关闭制冷模式 */
//         }
//     }

// }

// /**
//  * @brief  功能文本标签
//  * @param  无
//  * @return 无
//  */
// static void lv_example_label(void)
// {
//     /* 根据活动屏幕宽度选择字体 */
//     if (scr_act_width() <= 320)
//     {
//         font = &lv_font_montserrat_14;
//     }
//     else if (scr_act_width() <= 480)
//     {
//         font = &lv_font_montserrat_14;
//     }
//     else
//     {
//         font = &lv_font_montserrat_20;
//     }
//     lv_obj_t *label = lv_label_create(lv_scr_act());                            /* 创建标签 */
//     lv_label_set_text(label, "Control  Center");                                /* 设置文本内容 */
//     lv_obj_set_style_text_font(label, font, LV_STATE_DEFAULT);                  /* 设置字体 */
//     lv_obj_align(label, LV_ALIGN_CENTER, 0, -scr_act_height() / 3 );            /* 设置位置 */
// }

// /**
//  * @brief  制冷功能开关
//  * @param  无
//  * @return 无
//  */
// static void lv_example_switch1(void)
// {
//     /* 制冷模式基础对象（矩形背景） */
//     lv_obj_t *obj_cool = lv_obj_create(lv_scr_act());                               /* 创建基础对象 */
//     lv_obj_set_size(obj_cool,scr_act_height() / 3, scr_act_height() / 3 );          /* 设置大小 */
//     lv_obj_align(obj_cool, LV_ALIGN_CENTER, -scr_act_width() / 4, 0 );              /* 设置位置 */

//     /* 制冷模式开关标签 */
//     lv_obj_t *label_cool = lv_label_create(obj_cool);                               /* 创建标签 */
//     lv_label_set_text(label_cool, "Cool");                                          /* 设置文本内容 */
//     lv_obj_set_style_text_font(label_cool, font, LV_STATE_DEFAULT);                 /* 设置字体 */
//     lv_obj_align(label_cool, LV_ALIGN_CENTER, 0, -scr_act_height() / 16 );          /* 设置位置 */

//     /* 制冷模式开关 */
//     switch_cool = lv_switch_create(obj_cool);                                       /* 创建开关 */
//     lv_obj_set_size(switch_cool,scr_act_height() / 6, scr_act_height() / 12 );      /* 设置大小 */
//     lv_obj_align(switch_cool, LV_ALIGN_CENTER, 0, scr_act_height() / 16 );          /* 设置位置 */
//     lv_obj_add_event_cb(switch_cool, switch_event_cb, LV_EVENT_VALUE_CHANGED, NULL);/* 添加事件 */
// }

// /**
//  * @brief  制暖功能开关
//  * @param  无
//  * @return 无
//  */
// static void lv_example_switch2(void)
// {
//     /* 制暖模式基础对象（矩形背景） */
//     lv_obj_t *obj_heat = lv_obj_create(lv_scr_act());
//     lv_obj_set_size(obj_heat,scr_act_height() / 3, scr_act_height() / 3 );
//     lv_obj_align(obj_heat, LV_ALIGN_CENTER, 0, 0 );

//     /* 制暖模式开关标签 */
//     lv_obj_t *label_heat = lv_label_create(obj_heat);
//     lv_label_set_text(label_heat, "Heat");
//     lv_obj_set_style_text_font(label_heat, font, LV_STATE_DEFAULT);
//     lv_obj_align(label_heat, LV_ALIGN_CENTER, 0, -scr_act_height() / 16 );

//     /* 制暖模式开关 */
//     switch_heat = lv_switch_create(obj_heat);
//     lv_obj_set_size(switch_heat,scr_act_height() / 6, scr_act_height() / 12 );
//     lv_obj_align(switch_heat, LV_ALIGN_CENTER, 0, scr_act_height() / 16 );
//     lv_obj_add_event_cb(switch_heat, switch_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
// }

// /**
//  * @brief  干燥功能开关
//  * @param  无
//  * @return 无
//  */
// static void lv_example_switch3(void)
// {
//     /* 干燥模式基础对象（矩形背景） */
//     lv_obj_t *obj_dry = lv_obj_create(lv_scr_act());
//     lv_obj_set_size(obj_dry,scr_act_height() / 3, scr_act_height() / 3 );
//     lv_obj_align(obj_dry, LV_ALIGN_CENTER, scr_act_width() / 4, 0 );

//     /* 干燥模式开关标签 */
//     lv_obj_t *label_dry = lv_label_create(obj_dry);
//     lv_label_set_text(label_dry, "Dry");
//     lv_obj_set_style_text_font(label_dry, font, LV_STATE_DEFAULT);
//     lv_obj_align(label_dry, LV_ALIGN_CENTER, 0, -scr_act_height() / 16 );

//     /* 干燥模式开关 */
//     switch_dry = lv_switch_create(obj_dry);
//     lv_obj_set_size(switch_dry,scr_act_height() / 6, scr_act_height() / 12 );
//     lv_obj_align(switch_dry, LV_ALIGN_CENTER, 0, scr_act_height() / 16 );
//     lv_obj_add_state(switch_dry, LV_STATE_CHECKED|LV_STATE_DISABLED);
// }

// /**
//  * @brief  LVGL演示
//  * @param  无
//  * @return 无
//  */
// void lv_mainstart(void)
// {
//     lv_example_label();         /* 功能标题 */
//     lv_example_switch1();       /* 制冷模式开关 */
//     lv_example_switch2();       /* 制暖模式开关 */
//     lv_example_switch3();       /* 干燥模式开关 */
// }

/*------------- 开关 -------------*/

/*------------- 表格 -------------*/
// /* 获取当前活动屏幕的宽高 */
// #define scr_act_width() lv_obj_get_width(lv_scr_act())
// #define scr_act_height() lv_obj_get_height(lv_scr_act())

// /**
//  * @brief  表格实例
//  * @param  无
//  * @return 无
//  */
// static void lv_example_table(void)
// {
//     /* 标题 */
//     lv_obj_t *label_title = lv_label_create(lv_scr_act());                                  /* 创建标题 */
//     lv_obj_align(label_title, LV_ALIGN_TOP_MID, 0, scr_act_height()/8);                     /* 设置位置 */
//     lv_obj_set_style_text_font(label_title, &lv_font_montserrat_20, LV_STATE_DEFAULT);      /* 设置字体 */
//     lv_label_set_text(label_title, "Today's prices");                                       /* 设置文本内容 */

//     /* 表格 */
//     lv_obj_t *table = lv_table_create(lv_scr_act());                                        /* 创建表格 */
//     lv_obj_set_height(table, scr_act_height()/2);                                           /* 设置表格总的高度 */
//     lv_obj_center(table);                                                                   /* 设置位置 */

//     /* 设置第1列单元格内容（名称） */
//     lv_table_set_cell_value(table, 0, 0, "Name");
//     lv_table_set_cell_value(table, 1, 0, "Apple");
//     lv_table_set_cell_value(table, 2, 0, "Banana");
//     lv_table_set_cell_value(table, 3, 0, "Lemon");
//     lv_table_set_cell_value(table, 4, 0, "Grape");
//     lv_table_set_cell_value(table, 5, 0, "Melon");
//     lv_table_set_cell_value(table, 6, 0, "Peach");
//     lv_table_set_cell_value(table, 7, 0, "Nuts");

//     /* 设置第2列单元格内容（价格） */
//     lv_table_set_cell_value(table, 0, 1, "Price");
//     lv_table_set_cell_value(table, 1, 1, "$7");
//     lv_table_set_cell_value(table, 2, 1, "$4");
//     lv_table_set_cell_value(table, 3, 1, "$6");
//     lv_table_set_cell_value(table, 4, 1, "$2");
//     lv_table_set_cell_value(table, 5, 1, "$5");
//     lv_table_set_cell_value(table, 6, 1, "$1");
//     lv_table_set_cell_value(table, 7, 1, "$9");

//     /* 单元格宽度 */
//     lv_table_set_col_width(table, 0, scr_act_width()/3);
//     lv_table_set_col_width(table, 1, scr_act_width()/3);
// }

// /**
//  * @brief  LVGL演示
//  * @param  无
//  * @return 无
//  */
// void lv_mainstart(void)
// {
//     lv_example_table();
// }

/*------------- 表格 -------------*/



/*------------- 密码输入 -------------*/
// /* 获取当前活动屏幕的宽高 */
// #define scr_act_width()  lv_obj_get_width(lv_scr_act())
// #define scr_act_height() lv_obj_get_height(lv_scr_act())

// static const lv_font_t *font;       /* 定义字体 */

// static lv_obj_t *keyboard;          /* 键盘 */
// static lv_obj_t *label_name;        /* 用户名正误提示标签 */
// static lv_obj_t *label_pass;        /* 密码正误提示标签 */

// /**
//  * @brief  用户名文本框事件回调
//  * @param  *e ：事件相关参数的集合，它包含了该事件的所有数据
//  * @return 无
//  */
// static void name_event_cb(lv_event_t *e)
// {
//     lv_event_code_t code = lv_event_get_code(e);            /* 获取事件类型 */
//     lv_obj_t *target = lv_event_get_target(e);              /* 获取触发源 */

//     if(code == LV_EVENT_FOCUSED)                            /* 事件类型：被聚焦 */
//     {
//         lv_keyboard_set_textarea(keyboard, target);         /* 关联用户名文本框和键盘 */
//     }
//     else if(code == LV_EVENT_VALUE_CHANGED)                 /* 事件类型：文本框的内容发生变化 */
//     {
//         const char *txt = lv_textarea_get_text(target);     /* 获取文本框的文本 */

//         if(strcmp(txt,"admin") == 0)                        /* 判断用户名是否正确 */
//         {
//             lv_label_set_text(label_name, LV_SYMBOL_OK);    /* 用户名正确，显示√ */
//         }
//         else
//         {
//             lv_label_set_text(label_name, "");              /* 用户名错误，不提示 */
//         }
//     }
// }

// /**
//  * @brief  密码文本框事件回调
//  * @param  *e ：事件相关参数的集合，它包含了该事件的所有数据
//  * @return 无
//  */
// static void pass_event_cb(lv_event_t *e)
// {
//     lv_event_code_t code = lv_event_get_code(e);            /* 获取事件类型 */
//     lv_obj_t *target = lv_event_get_target(e);              /* 获取触发源 */

//     if(code == LV_EVENT_FOCUSED)                            /* 事件类型：被聚焦 */
//     {
//         lv_keyboard_set_textarea(keyboard, target);         /* 关联用户名文本框和键盘 */
//     }
//     else if(code == LV_EVENT_VALUE_CHANGED)                 /* 事件类型：文本框的内容发生变化 */
//     {
//         const char *txt = lv_textarea_get_text(target);     /* 获取文本框的文本 */

//         if(strcmp(txt,"123456") == 0)                       /* 判断密码是否正确 */
//         {
//             lv_label_set_text(label_pass, LV_SYMBOL_OK);    /* 密码正确，显示√ */
//         }
//         else
//         {
//             lv_label_set_text(label_pass, "");              /* 密码错误，不提示 */
//         }
//     }
// }

// /**
//  * @brief  用户登录实例
//  * @param  无
//  * @return 无
//  */
// static void lv_example_textarea(void)
// {

//     /* 根据屏幕大小设置字体 */
//     if (scr_act_width() <= 320)
//     {
//         font = &lv_font_montserrat_12;
//     }
//     else if (scr_act_width() <= 480)
//     {
//         font = &lv_font_montserrat_14;
//     }
//     else
//     {
//         font = &lv_font_montserrat_22;
//     }

//     /* 用户名文本框 */
//     lv_obj_t *textarea_name = lv_textarea_create(lv_scr_act());                                     /* 创建文本框 */
//     lv_obj_set_width(textarea_name, scr_act_width()/2);                                             /* 设置宽度 */
//     lv_obj_set_style_text_font(textarea_name, font, LV_PART_MAIN);                                  /* 设置字体 */
//     lv_obj_align(textarea_name, LV_ALIGN_CENTER, 0, -scr_act_height()/3 );                          /* 设置位置 */
//     lv_textarea_set_one_line(textarea_name, true);                                                  /* 设置单行模式 */
//     lv_textarea_set_max_length(textarea_name, 6);                                                   /* 设置输入字符的最大长度 */
//     lv_textarea_set_placeholder_text(textarea_name, "user name");                                   /* 设置占位符 */
//     lv_obj_add_event_cb(textarea_name, name_event_cb, LV_EVENT_ALL, NULL);                          /* 添加文本框事件回调 */

//     /* 用户名正误提示标签 */
//     label_name = lv_label_create(lv_scr_act());                                                     /* 创建标签 */
//     lv_label_set_text(label_name, "");                                                              /* 默认不提示 */
//     lv_obj_set_style_text_font(label_name, font, LV_PART_MAIN);                                     /* 设置字体 */
//     lv_obj_align_to(label_name, textarea_name, LV_ALIGN_OUT_RIGHT_MID, 5, 0);                       /* 设置位置 */

//     /* 密码文本框 */
//     lv_obj_t *textarea_pass = lv_textarea_create(lv_scr_act());                                     /* 创建文本框 */
//     lv_obj_set_width(textarea_pass, scr_act_width()/2);                                             /* 设置宽度 */
//     lv_obj_set_style_text_font(textarea_pass, font, LV_PART_MAIN);                                  /* 设置字体 */
//     lv_obj_align_to(textarea_pass, textarea_name, LV_ALIGN_OUT_BOTTOM_MID, 0, scr_act_height()/20); /* 设置位置 */
//     lv_textarea_set_one_line(textarea_pass, true);                                                  /* 设置单行模式 */
//     lv_textarea_set_password_mode(textarea_pass, true);                                             /* 设置密码模式 */
//     lv_textarea_set_password_show_time(textarea_pass, 1000);                                        /* 设置密码显示时间 */
//     lv_textarea_set_max_length(textarea_pass, 8);                                                   /* 设置输入字符的最大长度 */
//     lv_textarea_set_placeholder_text(textarea_pass, "password");                                    /* 设置占位符 */
//     lv_obj_add_event_cb(textarea_pass, pass_event_cb, LV_EVENT_ALL, NULL);                          /* 添加文本框事件回调 */

//     /* 密码正误提示标签 */
//     label_pass = lv_label_create(lv_scr_act());                                                     /* 创建标签 */
//     lv_label_set_text(label_pass, "");                                                              /* 默认不提示 */
//     lv_obj_set_style_text_font(label_pass, font, LV_PART_MAIN);                                     /* 设置字体 */
//     lv_obj_align_to(label_pass, textarea_pass, LV_ALIGN_OUT_RIGHT_MID, 5, 0);                       /* 设置位置 */

//     /* 键盘 */
//     keyboard = lv_keyboard_create(lv_scr_act());                                                    /* 创建键盘 */
//     lv_obj_set_size(keyboard, scr_act_width(), scr_act_height()/2);                                 /* 设置大小 */
// }

// /**
//  * @brief  LVGL演示
//  * @param  无
//  * @return 无
//  */
// void lv_mainstart(void)
// {
//     lv_example_textarea();
// }

/*------------- 密码输入 -------------*/



/*------------- 日历 -------------*/

// LV_FONT_DECLARE(myFont14) \
// LV_FONT_DECLARE(myFont24)

// // #if LV_CALENDAR_WEEK_STARTS_MONDAY == 0
// // #error LV_CALENDAR_WEEK_STARTS_MONDAY MUST SET 1.
// // #endif

// /* 获取当前活动屏幕的宽高 */
// #define scr_act_width() lv_obj_get_width(lv_scr_act())
// #define scr_act_height() lv_obj_get_height(lv_scr_act())

// lv_obj_t* calendar;

// /**
//  * @brief  日历回调
//  * @param  无
//  * @return 无
//  */
// static void event_calendar_cb(lv_event_t* e)
// {
//     lv_event_code_t code = lv_event_get_code(e);
//     lv_obj_t* obj = lv_event_get_target(e);
//     lv_obj_t* label = (lv_obj_t*)e->user_data;
//     lv_calendar_date_t date_temp;
//     char buf[11];

//     if (LV_EVENT_VALUE_CHANGED == code)
//     {
//         if (LV_RES_OK == lv_calendar_get_pressed_date(calendar, &date_temp))
//          {
//             lv_calendar_set_today_date(calendar, date_temp.year, date_temp.month, date_temp.day);
//             lv_snprintf(buf, sizeof(buf), "%d/%02d/%02d", date_temp.year, date_temp.month, date_temp.day);
//             lv_label_set_text(label, buf);
//         }
//     }
// }

// static lv_calendar_date_t highlight_days[2];/* 定义的日期,必须用全局或静态定义 */

// /**
//  * @brief  例
//  * @param  无
//  * @return 无
//  */
// static void lv_example_calendar(void)
// {
//     /* 定义并初始化日历 */
//     calendar = lv_calendar_create(lv_scr_act());
//     /* 设置日历的大小 */
//     lv_obj_set_size(calendar, scr_act_height() * 0.85, scr_act_height()* 0.85);
//     lv_obj_center(calendar);
//     /* 设置日历的日期 */
//     lv_calendar_set_today_date(calendar, 2022, 4, 7);
//     /* 设置日历显示的月份 */
//     lv_calendar_set_showed_date(calendar, 2022, 4);
//     /* 设置日历头 */
//     lv_calendar_header_dropdown_create(calendar);


//     highlight_days[0].year = 2022;  /* 设置第一个日期 */
//     highlight_days[0].month = 4;
//     highlight_days[0].day = 5;
//     highlight_days[1].year = 2022;  /* 设置第二个日期 */
//     highlight_days[1].month = 4;
//     highlight_days[1].day = 6;

//     lv_calendar_set_highlighted_dates(calendar, highlight_days,3);
//     /* 更新日历参数 */
//     lv_obj_update_layout(calendar);


//     lv_obj_t* label = lv_label_create(lv_scr_act());                                /* 定义并创建标签 */
//     lv_obj_set_width(label, lv_obj_get_x(calendar));                                /* 设置标签宽度 */
//     lv_obj_align(label, LV_ALIGN_LEFT_MID, 0, 0);                                   /* 设置标签位置 */
//     lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);         /* 设置标签文本对齐方式 */
//     lv_label_set_text(label, "Wait input...");                                      /* 设置标签文本 */

//     lv_obj_add_event_cb(calendar, event_calendar_cb, LV_EVENT_ALL, label);           /* 设置日历回调 */
// }

// /**
//  * @brief  LVGL演示
//  * @param  无
//  * @return 无
//  */
// void lv_mainstart(void)
// {
//     lv_example_calendar();
// }

/*------------- 日历 -------------*/


/*------------- 图表 -------------*/
// LV_FONT_DECLARE(myFont14) \
// LV_FONT_DECLARE(myFont24)

// /* 获取当前活动屏幕的宽高 */
// #define scr_act_width() lv_obj_get_width(lv_scr_act())
// #define scr_act_height() lv_obj_get_height(lv_scr_act())

// /* 示例数据 */
// static const lv_coord_t example_data[] = {
//     25, 26, 27, 29, 30, 31, 32, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 
//     45, 46, 47, 47, 48, 48, 49, 49, 49, 50, 50, 50, 50, 50, 50, 50, 50, 49, 49, 
//     49, 48, 48, 47, 47, 46, 45, 44, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 33, 
//     32, 31, 30, 29, 27, 26, 25, 24, 22, 21, 20, 19, 17, 16, 15, 14, 13, 12, 11, 
//     10, 9, 8, 7, 6, 5, 5, 4, 3, 3, 2, 2, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 
//     1, 2, 2, 3, 4, 4, 5, 6, 6, 7, 8, 9, 10, 11, 12, 13, 15, 16, 17, 18, 19, 20, 
//     22, 23, 24, 25, 27, 28, 29, 30, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 
//     43, 44, 45, 46, 46, 47, 47, 48, 48, 49, 49, 49, 50, 50, 50, 50, 50, 50, 50, 
//     50, 49, 49, 49, 48, 48, 47, 46, 46, 45, 44, 43, 42, 42, 41, 40, 39, 38, 36, 
//     35, 34, 33, 32, 31, 29, 28, 27, 26, 24, 23, 22, 21, 19, 18, 17, 16, 15, 14, 
//     12, 11, 10, 9, 8, 8, 7, 6, 5, 4, 4, 3, 2, 2, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 
//     0, 1, 1, 1, 2, 2, 3, 3, 4, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 
//     18, 20, 21, 22, 23, 25, 26, 27, 28, 30, 31, 32, 33, 34, 36, 37, 38, 39, 40, 
//     41, 42, 43, 44, 44, 45, 46, 46, 47, 48, 48, 49, 49, 49, 50, 50, 50, 50, 50, 
//     50, 50, 50, 49, 49, 49, 48, 48, 47, 47, 46, 45, 45, 44, 43, 42, 41, 40, 39, 
//     38, 37, 36, 35, 34, 33, 31, 30, 29, 28, 26, 25, 24, 23, 21, 20, 19, 18, 17, 
//     15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 6, 5, 4, 3, 3, 2, 2, 1, 1, 1, 0, 0, 0, 
//     0, 0, 0, 0, 0, 1, 1, 1, 2, 2, 3, 3, 4, 5, 5, 6, 7, 8, 9, 10, 11, 12, 13, 
//     14, 15, 16, 18, 19, 20, 21, 23, 24, 25, 26, 28, 29, 30, 31, 32, 34, 35, 36, 
//     37, 38, 39, 40, 41, 42, 43, 44, 45, 45, 46, 47, 47, 48, 48, 49, 49, 49, 50, 
//     50, 50, 50, 50, 50, 50, 50, 49, 49, 49, 48, 48, 47, 47, 46, 45, 44, 44, 43, 
//     42, 41, 40, 39, 38, 37, 36, 35, 33, 32, 31, 30, 29, 27, 26, 25, 24, 22, 21, 
//     20, 19, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 5, 4, 3, 3, 2, 2, 1, 
//     1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 2, 2, 3, 4, 4, 5, 6, 7, 7, 8, 9, 10, 
//     11, 12, 13, 15, 16, 17, 18, 19, 20, 22, 23, 24, 25, 27, 28, 29, 30, 32, 33, 
//     34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 46, 47, 47, 48, 48, 49, 
//     49, 49, 50, 50, 50, 50, 50, 50, 50, 50, 49, 49, 49, 48, 48, 47, 46, 46, 45, 
//     44, 43, 42, 42, 41, 40, 39, 38, 36, 35, 34, 33, 32, 31, 29, 28, 27, 26, 24, 
//     23, 22, 21, 19, 18, 17, 16, 15, 14, 12, 11, 10, 9, 8, 7, 7, 6, 5, 4, 4, 3, 
//     2, 2, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 2, 2, 3, 3, 4, 4, 5, 6, 7, 
//     8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 20, 21, 22, 23, 25, 26, 27, 28, 
//     30, 31, 32, 33, 34, 36, 37, 38, 39, 40, 41, 42, 43, 44, 44, 45, 46, 46, 47, 
//     48, 48, 49, 49, 49, 50, 50, 50, 50, 50, 50, 50, 50, 49, 49, 49, 48, 48, 47, 
//     47, 46, 45, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 31, 30, 29, 
//     28, 26, 25, 24, 23, 21, 20, 19, 18, 17, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 
//     6, 5, 4, 3, 3, 2, 2, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 2, 2, 3, 3, 
//     4, 5, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 18, 19, 20, 21, 23, 24, 25, 
//     26, 28, 29, 30, 31, 32, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 45, 
//     46, 47, 47, 48, 48, 49, 49, 49, 50, 50, 50, 50, 50, 50, 50, 50, 49, 49, 49, 
//     48, 48, 47, 47, 46, 45, 44, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 33, 32, 
//     31, 30, 29, 27, 26, 25, 24, 22, 21, 20, 19, 17, 16, 15, 14, 13, 12, 11, 10, 
//     9, 8, 7, 6, 5, 5, 4, 3, 3, 2, 2, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 
//     2, 2, 3, 4, 4, 5, 6, 7, 7, 8, 9, 10, 11, 12, 13, 15, 16, 17, 18, 19, 20, 
//     22, 23, 24, 25, 27, 28, 29, 30, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 
//     43, 44, 45, 46, 46, 47, 47, 48, 48, 49, 49, 49, 50, 50, 50, 50, 50, 50, 50, 
//     50, 49, 49, 49, 48, 48, 47, 46, 46, 45, 44, 43, 42, 42, 41, 40, 39, 38, 36, 
//     35, 34, 33, 32, 31, 29, 28, 27, 26, 24, 23, 22, 21, 19, 18, 17, 16, 15, 14, 
//     12, 11, 10, 9, 8, 7, 7, 6, 5, 4, 4, 3, 2, 2, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 
//     0, 1, 1, 1, 2, 2, 3, 3, 4, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 
// };

// static lv_obj_t* label;

// /**
//  * @brief  图表回调
//  * @param  无
//  * @return 无
//  */
// static void event_chart_cb(lv_event_t* e)
// {
//     lv_obj_t* chart = lv_event_get_target(e);
//     lv_event_code_t code = lv_event_get_code(e);
//     lv_chart_cursor_t* cursor = (lv_chart_cursor_t*)e->user_data;

//     if (LV_EVENT_VALUE_CHANGED == code) {
//         uint16_t point_id = lv_chart_get_pressed_point(chart);
//         if (LV_CHART_POINT_NONE != point_id) {
//             lv_chart_set_cursor_point(chart, cursor, NULL, point_id);
//             char buf[20];
//             lv_snprintf(buf, sizeof(buf), "(%d, %d)", point_id, example_data[point_id]);
//             lv_label_set_text(label, buf);
//         }
//     }
// }

// /**
//  * @brief  X轴滑杆回调
//  * @param  无
//  * @return 无
//  */
// static void event_slider_x_cb(lv_event_t* e)
// {
//     lv_obj_t* slider_x = lv_event_get_target(e);
//     lv_obj_t* chart = (lv_obj_t*)e->user_data;
//     lv_chart_set_zoom_x(chart, lv_slider_get_value(slider_x));
// }

// /**
//  * @brief  Y轴滑杆回调
//  * @param  无
//  * @return 无
//  */
// static void event_slider_y_cb(lv_event_t* e)
// {
//     lv_obj_t* slider_y = lv_event_get_target(e);
//     lv_obj_t* chart = (lv_obj_t*)e->user_data;
//     lv_chart_set_zoom_y(chart, lv_slider_get_value(slider_y));
// }

// /**
//  * @brief  例
//  * @param  无
//  * @return 无
//  */
// static void lv_example_chart(void)
// {
//     lv_obj_t* chart = lv_chart_create(lv_scr_act());                                                                        /* 定义并初始化图表 */
//     lv_obj_set_size(chart, scr_act_width(), scr_act_height());                                                              /* 设置图表大小 */
//     lv_obj_align(chart, LV_ALIGN_TOP_LEFT, 0, 0);                                                                           /* 设置图表位置 */
//     lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, -5, 55);                                                             /* 设置左轴Y方向的最大最小值 */
//     lv_obj_set_style_size(chart, 0, LV_PART_INDICATOR);                                                                     /* 不显示数据上的点 */
//     lv_chart_series_t* ser = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_RED), LV_CHART_AXIS_PRIMARY_Y);          /* 在图表中分配并添加数据系列 */
//     uint32_t pcnt = sizeof(example_data) / sizeof(example_data[0]);                                                         /* 计算数据线上的新点数 */
//     lv_chart_set_point_count(chart, pcnt);                                                                                  /* 在图表上设置数据线上的点数 */
//     lv_chart_set_ext_y_array(chart, ser, (lv_coord_t*)example_data);                                                        /* 为Y轴数据点设置一个外部阵列 */
//     lv_chart_cursor_t* cursor = lv_chart_add_cursor(chart, lv_palette_main(LV_PALETTE_BLUE), LV_DIR_LEFT | LV_DIR_BOTTOM);  /* 添加光标 */
//     lv_obj_update_layout(chart);                                                                                            /* 手动更新图表参数 */

//     label = lv_label_create(lv_scr_act());                                                                                  /* 定义并创建显示值的标签 */
//     lv_obj_align(label, LV_ALIGN_TOP_LEFT, 5, 5);                                                                           /* 设置标签的位置 */
//     lv_obj_set_style_text_font(label, &lv_font_montserrat_14, LV_PART_MAIN);                                                /* 设置标签的字体 */
//     lv_label_set_text(label, "");                                                                                           /* 设置标签的文本 */

//     lv_obj_t* slider_x = lv_slider_create(lv_scr_act());                                                                    /* 定义并创建X轴缩放滑杆 */
//     lv_slider_set_range(slider_x, LV_IMG_ZOOM_NONE, LV_IMG_ZOOM_NONE * 10);                                                 /* 设置滑杆范围 */
//     lv_obj_add_event_cb(slider_x, event_slider_x_cb, LV_EVENT_VALUE_CHANGED, chart);                                        /* 设置滑杆回调 */
//     lv_obj_set_width(slider_x, lv_obj_get_width(chart) - 3.5 * lv_obj_get_height(chart) / 10);                              /* 设置滑杆宽度 */
//     lv_obj_set_height(slider_x, lv_obj_get_height(chart) / 10);                                                             /* 设置滑杆高度 */
//     lv_obj_align_to(slider_x, chart, LV_ALIGN_BOTTOM_LEFT, lv_obj_get_height(chart) / 10, -lv_obj_get_height(chart) / 20);  /* 设置滑杆位置 */
    
//     lv_obj_t* slider_y = lv_slider_create(lv_scr_act());                                                                    /* 定义并创建Y轴缩放滑杆 */
//     lv_slider_set_range(slider_y, LV_IMG_ZOOM_NONE, LV_IMG_ZOOM_NONE * 10);                                                 /* 设置滑杆范围 */
//     lv_obj_add_event_cb(slider_y, event_slider_y_cb, LV_EVENT_VALUE_CHANGED, chart);                                        /* 设置滑杆回调 */
//     lv_obj_set_width(slider_y, lv_obj_get_height(chart) / 10);                                                              /* 设置滑杆宽度 */
//     lv_obj_set_height(slider_y, lv_obj_get_height(chart) - 3.5 * lv_obj_get_height(chart) / 10);                            /* 设置滑杆高度 */
//     lv_obj_align_to(slider_y, chart, LV_ALIGN_TOP_RIGHT, -lv_obj_get_height(chart) / 20, lv_obj_get_height(chart) / 10);    /* 设置滑杆位置 */

//     lv_obj_add_event_cb(chart, event_chart_cb, LV_EVENT_VALUE_CHANGED, cursor);                                             /* 设置图表回调 */
// }

// /**
//  * @brief  LVGL演示
//  * @param  无
//  * @return 无
//  */
// void lv_mainstart(void)
// {
//     lv_example_chart();
// }

/*------------- 图表 -------------*/

/*------------- 色环 -------------*/
// /* 获取当前活动屏幕的宽高 */
// #define scr_act_width() lv_obj_get_width(lv_scr_act())
// #define scr_act_height() lv_obj_get_height(lv_scr_act())

// static lv_obj_t* obj;

// /**
//  * @brief  色环事件回调
//  * @param  *e ：事件相关参数的集合，它包含了该事件的所有数据
//  * @return 无
//  */
// static void colorwheel_event_cb(lv_event_t *e)
// {
//     lv_obj_t *target = lv_event_get_target(e);                                          /* 获取触发源 */

//     lv_obj_set_style_bg_color(obj, lv_colorwheel_get_rgb(target), LV_PART_MAIN);        /* 设置基础对象背景颜色 */
// }

// /**
//  * @brief  色环部件实例
//  * @param  无
//  * @return 无
//  */
// static void lv_example_colorwheel(void)
// {
//     /* 色环（用于选择颜色） */
//     lv_obj_t *colorwheel = lv_colorwheel_create(lv_scr_act(), true);                    /* 创建色环 */
//     lv_obj_set_size(colorwheel, scr_act_height() * 2 / 3, scr_act_height()* 2 / 3);     /* 设置大小 */
//     lv_obj_center(colorwheel);                                                          /* 设置位置 */
//     lv_obj_set_style_arc_width(colorwheel, scr_act_height() * 0.1, LV_PART_MAIN);       /* 设置色环圆弧宽度 */
//     lv_colorwheel_set_mode_fixed(colorwheel, true);                                     /* 固定色环模式 */

//     /* 基础对象（用于显示所选颜色） */
//     obj = lv_obj_create(lv_scr_act());                                                  /* 创建基础对象 */
//     lv_obj_set_size(obj, scr_act_height() / 3, scr_act_height() / 3);                   /* 设置大小 */
//     lv_obj_align_to(obj, colorwheel, LV_ALIGN_CENTER, 0, 0);                            /* 设置位置 */
//     lv_obj_set_style_radius(obj, LV_RADIUS_CIRCLE, LV_PART_MAIN);                       /* 设置圆角 */
//     lv_obj_set_style_bg_color(obj, lv_colorwheel_get_rgb(colorwheel), LV_PART_MAIN);    /* 设置背景颜色 */
//     lv_obj_add_event_cb(colorwheel, colorwheel_event_cb, LV_EVENT_VALUE_CHANGED, NULL); /* 设置色环事件回调 */
// }

// /**
//  * @brief  LVGL演示
//  * @param  无
//  * @return 无
//  */
// void lv_mainstart(void)
// {
//     lv_example_colorwheel();
// }
/*------------- 色环 -------------*/

/*------------- 键盘 -------------*/
// /* 获取当前活动屏幕的宽高 */
// #define scr_act_width()  lv_obj_get_width(lv_scr_act())
// #define scr_act_height() lv_obj_get_height(lv_scr_act())

// /**
//  * @brief  键盘事件回调
//  * @param  *e ：事件相关参数的集合，它包含了该事件的所有数据
//  * @return 无
//  */
// static void keyboard_event_cb(lv_event_t *e)
// {
//     lv_event_code_t code = lv_event_get_code(e);                            /* 获取事件类型 */
//     lv_obj_t *target = lv_event_get_target(e);                              /* 获取触发源 */

//     if(code == LV_EVENT_VALUE_CHANGED)
//     {
//         uint16_t id = lv_btnmatrix_get_selected_btn(target);                /* 获取键盘按钮索引 */

//         const char *txt = lv_btnmatrix_get_btn_text(target, id);            /* 获取按钮文本 */

//         if(strcmp(txt, LV_SYMBOL_KEYBOARD) == 0)                            /* 判断是不是键盘图标被按下 */
//         {
//             if(lv_keyboard_get_mode(target) == LV_KEYBOARD_MODE_NUMBER)     /* 获取当前键盘模式，判断是否为数字模式 */
//             {
//                 lv_keyboard_set_mode(target, LV_KEYBOARD_MODE_TEXT_LOWER);  /* 如果是数字模式，则切换为小写字母模式 */
//             }
//             else
//             {
//                 lv_keyboard_set_mode(target, LV_KEYBOARD_MODE_NUMBER);      /* 不是数字模式，则切换为数字模式 */
//             }
//         }
//     }
// }

// /**
//  * @brief  键盘实例
//  * @param  无
//  * @return 无
//  */
// static void lv_example_keyboard(void)
// {
//     /* 文本框 */
//     lv_obj_t *textarea = lv_textarea_create(lv_scr_act());                              /* 创建文本框 */
//     lv_obj_set_size(textarea, scr_act_width() - 10, scr_act_height() / 2 - 10);         /* 设置大小 */
//     lv_obj_align(textarea, LV_ALIGN_TOP_MID, 0, 0);                                     /* 设置位置 */
    
//     /* 键盘 */
//     lv_obj_t *keyboard = lv_keyboard_create(lv_scr_act());                              /* 创建键盘 */
//     lv_keyboard_set_textarea(keyboard, textarea);                                       /* 关联键盘和文本框 */
//     lv_obj_add_event_cb(keyboard, keyboard_event_cb, LV_EVENT_VALUE_CHANGED, NULL);     /* 设置键盘事件回调 */
// }

// /**
//  * @brief  LVGL演示
//  * @param  无
//  * @return 无
//  */
// void lv_mainstart(void)
// {
//     lv_example_keyboard();
// }
/*------------- 键盘 -------------*/


/*------------- LED -------------*/
// /* 获取当前活动屏幕的宽高 */
// #define scr_act_width() lv_obj_get_width(lv_scr_act())
// #define scr_act_height() lv_obj_get_height(lv_scr_act())

// static const lv_font_t *font;                   /* 定义字体 */
// static lv_obj_t *obj;                           /* 基础对象 */

// lv_obj_t* led1, *led2, *led3;

// /**
//  * @brief  LED事件回调
//  * @param  *e ：事件相关参数的集合，它包含了该事件的所有数据
//  * @return 无
//  */
// static void led_event_cb(lv_event_t* e)
// {
//     lv_obj_t* led = lv_event_get_target(e);     /* 获取触发源 */
//     lv_led_toggle(led);                         /* 翻转LED状态 */
// 	if (led == led1)
// 		LEDR_TOGGLE;
// 	else if (led == led2)
// 		LEDG_TOGGLE;
// 	else if (led == led3)
// 		LEDR_TOGGLE;
// }

// /**
//  * @brief  LED1
//  * @param  无
//  * @return 无
//  */
// static void lv_example_led_1(void)
// {
//     /* 根据活动屏幕宽度选择字体 */
//     if (scr_act_width() <= 480)
//     {
//         font = &lv_font_montserrat_14;
//     }
//     else
//     {
//         font = &lv_font_montserrat_20;
//     }

//     /* 创建基础对象作为背景 */
//     obj = lv_obj_create(lv_scr_act());
//     lv_obj_set_size(obj, scr_act_width() * 5 /6 , scr_act_height() * 3 /5);
//     lv_obj_align(obj, LV_ALIGN_CENTER, 0 , 0);
//     lv_obj_set_style_bg_color(obj, lv_color_hex(0xefefef), LV_STATE_DEFAULT);

//     led1 = lv_led_create(obj);                                                     /* 创建LED */
//     lv_obj_set_size(led1, scr_act_height() /5 , scr_act_height() /5);                        /* 设置LED大小 */
//     lv_obj_align(led1, LV_ALIGN_CENTER, -scr_act_width() * 4/ 15, -scr_act_height() /15);    /* 设置LED位置 */

//     lv_led_off(led1);                                                                        /* 关闭LED */
// 	LEDR(0);LEDG(0);LEDB(0);

//     lv_obj_add_event_cb(led1, led_event_cb, LV_EVENT_CLICKED, NULL);                         /* 设置LED事件回调 */

//     lv_obj_t *label = lv_label_create(lv_scr_act());                                        /* 创建LED功能标签 */
//     lv_label_set_text(label, "ROOM 1");                                                     /* 设置文本 */
//     lv_obj_set_style_text_font(label, font, LV_STATE_DEFAULT);                              /* 设置字体 */
//     lv_obj_align_to(label, led1, LV_ALIGN_OUT_BOTTOM_MID, 0, scr_act_height() /15 );         /* 设置位置 */
// }

// /**
//  * @brief  LED2
//  * @param  无
//  * @return 无
//  */
// static void lv_example_led_2(void)
// {
//     led2 = lv_led_create(obj);                                                     /* 创建LED */
//     lv_obj_set_size(led2, scr_act_height() /5 , scr_act_height() /5);                        /* 设置LED大小 */
//     lv_obj_align(led2, LV_ALIGN_CENTER, 0, -scr_act_height() /15);                           /* 设置LED位置 */
//     lv_led_set_color(led2, lv_color_hex(0xff0000));                                          /* 设置LED颜色 */

//     lv_led_on(led2);                                                                         /* 打开LED */
//     LEDR(1);LEDG(0);LEDB(0);

// 	lv_obj_add_event_cb(led2, led_event_cb, LV_EVENT_CLICKED, NULL);                         /* 设置LED事件回调 */

//     lv_obj_t *label = lv_label_create(lv_scr_act());                                        /* 创建LED功能标签 */
//     lv_label_set_text(label, "ROOM 2");                                                     /* 设置文本 */
//     lv_obj_set_style_text_font(label, font, LV_STATE_DEFAULT);                              /* 设置字体 */
//     lv_obj_align_to(label, led2, LV_ALIGN_OUT_BOTTOM_MID, 0, scr_act_height() /15 );         /* 设置位置 */
// }

// /**
//  * @brief  LED3
//  * @param  无
//  * @return 无
//  */
// static void lv_example_led_3(void)
// {
//     led3 = lv_led_create(obj);                                                     /* 创建LED */
//     lv_obj_set_size(led3, scr_act_height() /5 , scr_act_height() /5);                        /* 设置LED大小 */
//     lv_obj_align(led3, LV_ALIGN_CENTER, scr_act_width() * 4/ 15, -scr_act_height() /15);     /* 设置LED位置 */
//     lv_led_set_color(led3, lv_color_hex(0x2fc827));                                          /* 设置LED颜色 */
    
// 	lv_led_off(led3); 
// 	LEDR(0);LEDG(0);LEDB(0);
// 	                                                                       /* 关闭LED */
//     lv_obj_add_event_cb(led3, led_event_cb, LV_EVENT_CLICKED, NULL);                         /* 设置LED事件回调 */

//     lv_obj_t *label = lv_label_create(lv_scr_act());                                        /* 创建LED功能标签 */
//     lv_label_set_text(label, "ROOM 3");                                                     /* 设置文本 */
//     lv_obj_set_style_text_font(label, font, LV_STATE_DEFAULT);                              /* 设置字体 */
//     lv_obj_align_to(label, led3, LV_ALIGN_OUT_BOTTOM_MID, 0, scr_act_height() /15 );         /* 设置位置 */
// }

// /**
//  * @brief  LVGL演示
//  * @param  无
//  * @return 无
//  */
// void lv_mainstart(void)
// {
//     lv_example_led_1();         /* LED1 */
//     lv_example_led_2();         /* LED2 */
//     lv_example_led_3();         /* LED3 */
// }
/*------------- LED -------------*/

/*------------- 列表 -------------*/
// /* 获取当前活动屏幕的宽高 */
// #define scr_act_width() lv_obj_get_width(lv_scr_act())
// #define scr_act_height() lv_obj_get_height(lv_scr_act())

// static lv_obj_t *list;              /* 定义列表 */
// static lv_obj_t *list_label;        /* 定义标签 */
// static const lv_font_t *font;       /* 定义字体 */

// /**
//  * @brief  列表按钮事件回调
//  * @param  *e ：事件相关参数的集合，它包含了该事件的所有数据
//  * @return 无
//  */
// static void list_btn_event_cb(lv_event_t *e)
// {
//     lv_obj_t *list_btn = lv_event_get_target(e);                                   /* 获取触发源 */

//     lv_label_set_text(list_label, lv_list_get_btn_text(list, list_btn));           /* 获取按钮文本并显示 */
    
//     lv_obj_add_state(list_btn, LV_STATE_FOCUS_KEY);                                /* 添加状态（聚焦） */
// }

// /**
//  * @brief  例
//  * @param  无
//  * @return 无
//  */
// static void lv_example_list(void)
// {
//     /* 根据屏幕大小设置字体 */
//     if (scr_act_width() <= 320)
//     {
//         font = &lv_font_montserrat_14;
//     }
//     else if (scr_act_width() <= 480)
//     {
//         font = &lv_font_montserrat_14;
//     }
//     else
//     {
//         font = &lv_font_montserrat_14;
//     }

//     /* 创建左侧矩形背景 */
//     lv_obj_t* obj_left = lv_obj_create(lv_scr_act());                               /* 创建一个基础对象 */
//     lv_obj_set_width(obj_left, scr_act_width() * 0.7);                              /* 设置宽度 */
//     lv_obj_set_height(obj_left, scr_act_height() * 0.9);                            /* 设置高度 */
//     lv_obj_align(obj_left, LV_ALIGN_LEFT_MID, 5, 0);                                /* 设置位置 */
//     lv_obj_update_layout(obj_left);                                                 /* 手动更新物体的参数 */

//     /* 创建右侧矩形背景 */
//     lv_obj_t* obj_right = lv_obj_create(lv_scr_act());                              /* 创建一个基础对象 */
//     lv_obj_set_width(obj_right, scr_act_width() - lv_obj_get_width(obj_left) - 15); /* 设置宽度 */
//     lv_obj_set_height(obj_right, lv_obj_get_height(obj_left));                      /* 设置高度 */
//     lv_obj_align_to(obj_right, obj_left, LV_ALIGN_OUT_RIGHT_MID, 5, 0);             /* 设置位置 */
//     lv_obj_update_layout(obj_right);                                                /* 手动更新物体的参数 */

//     /* 显示当前选项的文本内容 */
//     list_label = lv_label_create(obj_right);                                        /* 创建标签 */
//     lv_obj_set_width(list_label, lv_obj_get_width(obj_right) - 13);                 /* 设置标签的宽度 */
//     lv_obj_align(list_label, LV_ALIGN_TOP_MID, 0, 5);                               /* 设置标签位置 */
//     lv_obj_update_layout(list_label);                                               /* 手动更新标签的参数 */
//     lv_obj_set_style_text_align(list_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);    /* 设置标签文本对齐方式 */
//     lv_label_set_text(list_label, "New");                                           /* 设置标签文本 */
//     lv_obj_set_style_text_font(list_label, font, LV_PART_MAIN);                     /* 设置标签文本字体 */

//     /* 创建列表 */
//     list = lv_list_create(obj_left);                                                /* 创建列表 */
//     lv_obj_set_width(list, lv_obj_get_width(obj_left) * 0.8);                       /* 设置列表宽度 */
//     lv_obj_set_height(list, lv_obj_get_height(obj_left) * 0.9);                     /* 设置列表高度 */
//     lv_obj_center(list);                                                            /* 设置列表的位置 */
//     lv_obj_set_style_text_font(list, font, LV_PART_MAIN);                           /* 设置字体 */

//     /* 为列表添加按钮 */
//     lv_obj_t* btn;
//     lv_list_add_text(list, "File");                                                 /* 添加列表文本 */
//     btn = lv_list_add_btn(list, LV_SYMBOL_FILE, "New");                             /* 添加按钮 */
//     lv_obj_add_event_cb(btn, list_btn_event_cb, LV_EVENT_CLICKED, NULL);            /* 添加按钮回调 */
//     btn = lv_list_add_btn(list, LV_SYMBOL_DIRECTORY, "Open");                       /* 添加按钮 */
//     lv_obj_add_event_cb(btn, list_btn_event_cb, LV_EVENT_CLICKED, NULL);            /* 添加按钮回调 */
//     btn = lv_list_add_btn(list, LV_SYMBOL_SAVE, "Save");                            /* 添加按钮 */
//     lv_obj_add_event_cb(btn, list_btn_event_cb, LV_EVENT_CLICKED, NULL);            /* 添加按钮回调 */
//     btn = lv_list_add_btn(list, LV_SYMBOL_CLOSE, "Delete");                         /* 添加按钮 */
//     lv_obj_add_event_cb(btn, list_btn_event_cb, LV_EVENT_CLICKED, NULL);            /* 添加按钮回调 */
//     btn = lv_list_add_btn(list, LV_SYMBOL_EDIT, "Edit");                            /* 添加按钮 */
//     lv_obj_add_event_cb(btn, list_btn_event_cb, LV_EVENT_CLICKED, NULL);            /* 添加按钮回调 */

//     lv_list_add_text(list, "Connectivity");                                         /* 添加列表文本 */
//     btn = lv_list_add_btn(list, LV_SYMBOL_BLUETOOTH, "Bluetooth");                  /* 添加按钮 */
//     lv_obj_add_event_cb(btn, list_btn_event_cb, LV_EVENT_CLICKED, NULL);            /* 添加按钮回调 */
//     btn = lv_list_add_btn(list, LV_SYMBOL_GPS, "Navigation");                       /* 添加按钮 */
//     lv_obj_add_event_cb(btn, list_btn_event_cb, LV_EVENT_CLICKED, NULL);            /* 添加按钮回调 */
//     btn = lv_list_add_btn(list, LV_SYMBOL_USB, "USB");                              /* 添加按钮 */
//     lv_obj_add_event_cb(btn, list_btn_event_cb, LV_EVENT_CLICKED, NULL);            /* 添加按钮回调 */
//     btn = lv_list_add_btn(list, LV_SYMBOL_BATTERY_FULL, "Battery");                 /* 添加按钮 */
//     lv_obj_add_event_cb(btn, list_btn_event_cb, LV_EVENT_CLICKED, NULL);            /* 添加按钮回调 */
//     lv_list_add_text(list, "Exit");                                                 /* 添加列表文本 */
//     btn = lv_list_add_btn(list, LV_SYMBOL_OK, "Apply");                             /* 添加按钮 */
//     lv_obj_add_event_cb(btn, list_btn_event_cb, LV_EVENT_CLICKED, NULL);            /* 添加按钮回调 */
//     btn = lv_list_add_btn(list, LV_SYMBOL_CLOSE, "Close");                          /* 添加按钮 */
//     lv_obj_add_event_cb(btn, list_btn_event_cb, LV_EVENT_CLICKED, NULL);            /* 添加按钮回调 */
// }

// /**
//  * @brief  LVGL演示
//  * @param  无
//  * @return 无
//  */
// void lv_mainstart(void)
// {
//     lv_example_list();
// }

/*------------- 列表 -------------*/

/*------------- 仪表 -------------*/
// LV_FONT_DECLARE(myFont14) \
// LV_FONT_DECLARE(myFont24)

// /* 获取当前活动屏幕的宽高 */
// #define scr_act_width() lv_obj_get_width(lv_scr_act())
// #define scr_act_height() lv_obj_get_height(lv_scr_act())

// /**
//  * @brief  例1指针动画
//  * @param  无
//  * @return 无
//  */
// static void anim_1_indic_cb(lv_obj_t* meter, int32_t value)
// {
//     lv_meter_indicator_t* indic = (lv_meter_indicator_t*)meter->user_data;

//     lv_meter_set_indicator_value(meter, indic, value);   /* 动画值->刻度指针值 */
// }

// /**
//  * @brief  例1
//  * @param  无
//  * @return 无
//  */
// static void lv_example_meter_1(void)
// {
//     const lv_font_t* font;
//     /* 根据屏幕大小设置字体 */
//     if (scr_act_width() <= 320)
//         font = &lv_font_montserrat_14;
//     else if (scr_act_width() <= 480)
//         font = &lv_font_montserrat_14;
//     else
//         font = &lv_font_montserrat_16;

//     lv_obj_t* meter = lv_meter_create(lv_scr_act());                                                                    /* 定义并创建仪表 */
//     lv_obj_set_width(meter, scr_act_height() * 0.4);                                                                    /* 设置仪表宽度 */
//     lv_obj_set_height(meter, scr_act_height() * 0.4);                                                                   /* 设置仪表高度 */
//     lv_obj_align(meter, LV_ALIGN_CENTER, 0, 0);                                                      /* 设置仪表位置 */
//     lv_obj_set_style_text_font(meter, font, LV_PART_MAIN);                                                              /* 设置仪表文字字体 */
//     /* 设置仪表刻度 */
//     lv_meter_scale_t* scale = lv_meter_add_scale(meter);                                                                /* 定义并添加刻度 */
//     lv_meter_set_scale_ticks(meter, scale, 41, 1, scr_act_height() / 80, lv_palette_main(LV_PALETTE_GREY));             /* 设置小刻度 */
//     lv_meter_set_scale_major_ticks(meter, scale, 8, 1, scr_act_height() / 60, lv_color_black(), scr_act_height() / 30); /* 设置主刻度 */
//     /* 设置指针 */
//     lv_meter_indicator_t* indic;
//     indic = lv_meter_add_arc(meter, scale, 2, lv_palette_main(LV_PALETTE_BLUE), 0); /* 属性+作用范围 */
//     lv_meter_set_indicator_start_value(meter, indic, 0);
//     lv_meter_set_indicator_end_value(meter, indic, 20);

//     indic = lv_meter_add_scale_lines(meter, scale, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_BLUE), false, 0);
//     lv_meter_set_indicator_start_value(meter, indic, 0);
//     lv_meter_set_indicator_end_value(meter, indic, 20);

//     indic = lv_meter_add_arc(meter, scale, 2, lv_palette_main(LV_PALETTE_RED), 0);
//     lv_meter_set_indicator_start_value(meter, indic, 80);
//     lv_meter_set_indicator_end_value(meter, indic, 100);

//     indic = lv_meter_add_scale_lines(meter, scale, lv_palette_main(LV_PALETTE_RED), lv_palette_main(LV_PALETTE_RED), false, 0);
//     lv_meter_set_indicator_start_value(meter, indic, 80);
//     lv_meter_set_indicator_end_value(meter, indic, 100);

//     indic = lv_meter_add_needle_line(meter, scale, 4, lv_palette_main(LV_PALETTE_GREY), -10);
//     lv_obj_set_user_data(meter, indic);
//     /* 设置指针动画 */
//     lv_anim_t a;
//     lv_anim_init(&a);
//     lv_anim_set_exec_cb(&a,(lv_anim_exec_xcb_t)anim_1_indic_cb);
//     lv_anim_set_var(&a, meter); /* 动画继承仪表 */
//     lv_anim_set_values(&a, 0, 100);
//     lv_anim_set_time(&a, 2000);
//     lv_anim_set_repeat_delay(&a, 100);
//     lv_anim_set_playback_time(&a, 500);
//     lv_anim_set_playback_delay(&a, 100);
//     lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
//     lv_anim_start(&a);
// }

// static lv_obj_t* meter_2;

// /**
//  * @brief  例2指针动画
//  * @param  无
//  * @return 无
//  */
// static void anim_2_indic_cb(lv_meter_indicator_t* indic, int32_t value)
// {
//     lv_meter_set_indicator_end_value(meter_2, indic, value);
// }

// /**
//  * @brief  例2
//  * @param  无
//  * @return 无
//  */
// static void lv_example_meter_2(void)
// {
//     const lv_font_t* font;
//     /* 根据屏幕大小设置字体 */
//     if (scr_act_width() <= 320)
//         font = &lv_font_montserrat_14;
//     else if (scr_act_width() <= 480)
//         font = &lv_font_montserrat_14;
//     else
//         font = &lv_font_montserrat_14;

//     meter_2 = lv_meter_create(lv_scr_act());                                                                            /* 定义并创建仪表 */
//     lv_obj_set_width(meter_2, scr_act_height() * 0.4);                                                                  /* 设置仪表宽度 */
//     lv_obj_set_height(meter_2, scr_act_height() * 0.4);                                                                 /* 设置仪表高度 */
//     lv_obj_center(meter_2);                                                                                             /* 设置仪表位置 */
//     lv_obj_set_style_text_font(meter_2, font, LV_PART_MAIN);                                                            /* 设置仪表文字字体 */
//     lv_obj_remove_style(meter_2, NULL, LV_PART_INDICATOR);                                                              /* 删除仪表指针样式 */
//     /* 设置仪表刻度 */
//     lv_meter_scale_t* scale = lv_meter_add_scale(meter_2);
//     lv_meter_set_scale_ticks(meter_2, scale, 11, 2, scr_act_height() / 15, lv_palette_main(LV_PALETTE_GREY));
//     lv_meter_set_scale_major_ticks(meter_2, scale, 1, 2, scr_act_height() / 15, lv_color_hex3(0xEEE), scr_act_height() / 40);
//     lv_meter_set_scale_range(meter_2, scale, 0, 100, 270, 90);
//     /* 设置指针 */
//     lv_meter_indicator_t* indic1 = lv_meter_add_arc(meter_2, scale, scr_act_height() / 45, lv_palette_main(LV_PALETTE_RED), 0);
//     lv_meter_indicator_t* indic2 = lv_meter_add_arc(meter_2, scale, scr_act_height() / 45, lv_palette_main(LV_PALETTE_GREEN), -scr_act_height() / 45);
//     lv_meter_indicator_t* indic3 = lv_meter_add_arc(meter_2, scale, scr_act_height() / 45, lv_palette_main(LV_PALETTE_BLUE), -scr_act_height() / 22.5);
//     /* 设置指针动画 */
//     lv_anim_t a;
//     lv_anim_init(&a);
//     lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)anim_2_indic_cb);
//     lv_anim_set_values(&a, 0, 100);
//     lv_anim_set_repeat_delay(&a, 100);
//     lv_anim_set_playback_delay(&a, 100);
//     lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);

//     lv_anim_set_time(&a, 2000);
//     lv_anim_set_playback_time(&a, 500);
//     lv_anim_set_var(&a, indic1);
//     lv_anim_start(&a);

//     lv_anim_set_time(&a, 1000);
//     lv_anim_set_playback_time(&a, 1000);
//     lv_anim_set_var(&a, indic2);
//     lv_anim_start(&a);

//     lv_anim_set_time(&a, 1000);
//     lv_anim_set_playback_time(&a, 2000);
//     lv_anim_set_var(&a, indic3);
//     lv_anim_start(&a);
// }

// /**
//  * @brief  例3
//  * @param  无
//  * @return 无
//  */
// static void lv_example_meter_3(void)
// {
//     lv_obj_t* meter = lv_meter_create(lv_scr_act());                                                                    /* 定义并创建仪表 */
//     lv_obj_remove_style(meter, NULL, LV_PART_MAIN);                                                                     /* 移除背景样式 */
//     lv_obj_remove_style(meter, NULL, LV_PART_INDICATOR);                                                                /* 移除指针样式 */
//     lv_obj_set_width(meter, scr_act_height() * 0.4);                                                                    /* 设置仪表宽度 */
//     lv_obj_set_height(meter, scr_act_height() * 0.4);                                                                   /* 设置仪表高度 */
//     lv_obj_align(meter, LV_ALIGN_CENTER, scr_act_width() / 3, 0);                                                       /* 设置仪表位置 */
//     lv_obj_update_layout(meter);                                                                                        /* 手动更新参数 */
//     /* 设置仪表刻度 */
//     lv_meter_scale_t* scale = lv_meter_add_scale(meter);
//     lv_meter_set_scale_ticks(meter, scale, 0, 0, 0, lv_color_black());
//     lv_meter_set_scale_range(meter, scale, 0, 100, 360, 0);
//     /* 设置指针 */
//     lv_coord_t indic_w = scr_act_height() * 0.05;
//     lv_meter_indicator_t* indic1 = lv_meter_add_arc(meter, scale, indic_w, lv_palette_main(LV_PALETTE_ORANGE), 0);
//     lv_meter_set_indicator_start_value(meter, indic1, 0);
//     lv_meter_set_indicator_end_value(meter, indic1, 40);

//     lv_meter_indicator_t* indic2 = lv_meter_add_arc(meter, scale, indic_w, lv_palette_main(LV_PALETTE_YELLOW), 0);
//     lv_meter_set_indicator_start_value(meter, indic2, 40);  /*Start from the previous*/
//     lv_meter_set_indicator_end_value(meter, indic2, 80);

//     lv_meter_indicator_t* indic3 = lv_meter_add_arc(meter, scale, indic_w, lv_palette_main(LV_PALETTE_DEEP_ORANGE), 0);
//     lv_meter_set_indicator_start_value(meter, indic3, 80);  /*Start from the previous*/
//     lv_meter_set_indicator_end_value(meter, indic3, 100);
// }

// /**
//  * @brief  LVGL演示
//  * @param  无
//  * @return 无
//  */
// void lv_mainstart(void)
// {
//     lv_example_meter_1();
//     // lv_example_meter_2();
//     // lv_example_meter_3();
// }
/*------------- 仪表 -------------*/

/*------------- 消息框 -------------*/
// /* 获取当前活动屏幕的宽高 */
// #define scr_act_width()  lv_obj_get_width(lv_scr_act())
// #define scr_act_height() lv_obj_get_height(lv_scr_act())

// static lv_obj_t *msgbox;                                                            /* 消息框 */
// static lv_obj_t *slider_label;                                                      /* 音量百分比标签 */


// /*************************  第一部分 音量调节滑块  ******************************/

// /**
//  * @brief  滑块事件回调
//  * @param  *e ：事件相关参数的集合，它包含了该事件的所有数据
//  * @return 无
//  */
// static void slider_event_cb(lv_event_t *e)
// {
//     lv_obj_t *target = lv_event_get_target(e);                                      /* 获取触发源 */
//     lv_event_code_t code = lv_event_get_code(e);                                    /* 获取事件类型 */

//     if(code == LV_EVENT_VALUE_CHANGED)
//     {
//         lv_label_set_text_fmt(slider_label, "%d%%", lv_slider_get_value(target));   /* 获取当前值，更新音量百分比 */

//         if(lv_slider_get_value(target) > 80)                                        /* 音量大于80% */
//         {
//             lv_obj_clear_flag(msgbox, LV_OBJ_FLAG_HIDDEN);                          /* 清除消息框隐藏属性，出现弹窗提示 */
//         }
//     }
// }

// /**
//  * @brief  音量调节滑块
//  * @param  无
//  * @return 无
//  */
// static void lv_example_slider(void)
// {
//     /* 滑块 */
//     lv_obj_t * slider = lv_slider_create(lv_scr_act());                                 /* 创建滑块 */
//     lv_obj_set_size(slider, scr_act_width() / 2, 20);                                   /* 设置大小 */
//     lv_obj_center(slider);                                                              /* 设置位置 */
//     lv_slider_set_value(slider, 50, LV_ANIM_OFF);                                       /* 设置当前值 */
//     lv_obj_add_event_cb(slider, slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);         /* 添加事件 */

//     /* 百分比标签 */
//     slider_label = lv_label_create(lv_scr_act());                                       /* 创建百分比标签 */
//     lv_label_set_text(slider_label, "50%");                                             /* 设置文本内容 */
//     lv_obj_set_style_text_font(slider_label, &lv_font_montserrat_20, LV_STATE_DEFAULT); /* 设置字体 */
//     lv_obj_align_to(slider_label, slider, LV_ALIGN_OUT_RIGHT_MID, 20, 0);               /* 设置位置 */

//     /* 音量图标 */
//     lv_obj_t *sound_label = lv_label_create(lv_scr_act());                              /* 创建音量标签 */
//     lv_label_set_text(sound_label, LV_SYMBOL_VOLUME_MAX);                               /* 设置文本内容：音量图标 */
//     lv_obj_set_style_text_font(sound_label, &lv_font_montserrat_20, LV_STATE_DEFAULT);  /* 设置字体 */
//     lv_obj_align_to(sound_label, slider, LV_ALIGN_OUT_LEFT_MID, -20, 0);                /* 设置位置 */
// }


// /*************************  第二部分 消息框  ******************************/

// /**
//  * @brief  消息框事件回调
//  * @param  *e ：事件相关参数的集合，它包含了该事件的所有数据
//  * @return 无
//  */
// static void msgbox_event_cb(lv_event_t *e)
// {
//     lv_obj_t *target = lv_event_get_current_target(e);                 /* 获取当前触发源 */

//     if(lv_msgbox_get_active_btn(target) == 2)                          /* 获取按钮索引 */
//     {
//         lv_obj_add_flag(msgbox, LV_OBJ_FLAG_HIDDEN);                   /* 隐藏消息框 */
//     }
// }

// /**
//  * @brief  消息框实例
//  * @param  无
//  * @return 无
//  */
// static void lv_example_msgbox(void)
// {
//     static const char *btns[] = { " ", " ", "OK", "" };  // 按钮索引 0 1 2 3

//     /* 消息框整体 */
//     msgbox = lv_msgbox_create(lv_scr_act(),LV_SYMBOL_WARNING " Notice","Excessive volume may damage hearing.", btns,false);
//     lv_obj_set_size(msgbox, 300, 170);                                                       /* 设置大小 */
//     lv_obj_center(msgbox);                                                                   /* 设置位置 */
//     lv_obj_set_style_border_width(msgbox, 0, LV_STATE_DEFAULT);                              /* 去除边框 */
//     lv_obj_set_style_shadow_width(msgbox, 20, LV_STATE_DEFAULT);                             /* 设置阴影宽度 */
//     lv_obj_set_style_shadow_color(msgbox, lv_color_hex(0xa9a9a9), LV_STATE_DEFAULT);         /* 设置阴影颜色 */
//     lv_obj_set_style_pad_top(msgbox,18,LV_STATE_DEFAULT);                                    /* 设置顶部填充 */
//     lv_obj_set_style_pad_left(msgbox,20,LV_STATE_DEFAULT);                                   /* 设置左侧填充 */
//     lv_obj_add_event_cb(msgbox, msgbox_event_cb, LV_EVENT_VALUE_CHANGED, NULL);              /* 添加事件 */

//     /* 消息框标题 */
//     lv_obj_t *title = lv_msgbox_get_title(msgbox);                                           /* 获取标题部分 */
//     lv_obj_set_style_text_font(title, &lv_font_montserrat_20, LV_STATE_DEFAULT);             /* 设置字体 */
//     lv_obj_set_style_text_color(title, lv_color_hex(0xff0000),LV_STATE_DEFAULT);             /* 设置文本颜色：红色 */

//     /* 消息框主体 */
//     lv_obj_t *content = lv_msgbox_get_content(msgbox);                                       /* 获取主体部分 */
//     lv_obj_set_style_text_font(content, &lv_font_montserrat_20, LV_STATE_DEFAULT);           /* 设置字体 */
//     lv_obj_set_style_text_color(content, lv_color_hex(0x6c6c6c), LV_STATE_DEFAULT);          /* 设置文本颜色：灰色 */
//     lv_obj_set_style_pad_top(content,15,LV_STATE_DEFAULT);                                   /* 设置顶部填充 */

//     /* 消息框按钮 */
//     lv_obj_t *btn = lv_msgbox_get_btns(msgbox);                                              /* 获取按钮矩阵部分 */
//     lv_obj_set_style_bg_opa(btn, 0, LV_PART_ITEMS);                                          /* 设置按钮背景透明度 */
//     lv_obj_set_style_shadow_width(btn, 0, LV_PART_ITEMS);                                    /* 去除按钮阴影 */
//     lv_obj_set_style_text_font(btn, &lv_font_montserrat_20, LV_PART_ITEMS);                  /* 设置字体 */
//     lv_obj_set_style_text_color(btn, lv_color_hex(0x2271df),LV_PART_ITEMS);                  /* 设置文本颜色（未按下）：蓝色 */
//     lv_obj_set_style_text_color(btn, lv_color_hex(0xff0000),LV_PART_ITEMS|LV_STATE_PRESSED); /* 设置文本颜色（已按下）：红色 */

//     lv_obj_add_flag(msgbox, LV_OBJ_FLAG_HIDDEN);                                             /* 隐藏消息框 */
// }

// /**
//  * @brief  LVGL演示
//  * @param  无
//  * @return 无
//  */
// void lv_mainstart(void)
// {
//     lv_example_slider();
//     lv_example_msgbox();
// }

/*------------- 消息框 -------------*/

/*------------- SPAN -------------*/
// LV_FONT_DECLARE(myFont14) \
// LV_FONT_DECLARE(myFont24)
// /* 获取当前活动屏幕的宽高 */
// #define scr_act_width() lv_obj_get_width(lv_scr_act())
// #define scr_act_height() lv_obj_get_height(lv_scr_act())

// /**
//  * @brief  例
//  * @param  无
//  * @return 无
//  */
// static void lv_example_span(void)
// {
//     lv_obj_t* spangroup = lv_spangroup_create(lv_scr_act());                                        /* 定义并创建spangroup */
//     lv_obj_set_width(spangroup, 200);                                                               /* 设置spangroup宽度 */
//     lv_obj_set_height(spangroup, 200);                                                              /* 设置spangroup高度 */
//     lv_obj_center(spangroup);                                                                       /* 设置spangroup位置 */
//     lv_obj_set_style_border_width(spangroup, 1, LV_PART_MAIN);                                      /* 设置spangroup边框宽度 */
//     lv_obj_set_style_border_color(spangroup, lv_palette_main(LV_PALETTE_ORANGE), LV_PART_MAIN);     /* 设置spangroup边框颜色 */
//     lv_spangroup_set_align(spangroup, LV_TEXT_ALIGN_LEFT);                                          /* 设置spangroup文本对齐方式 */
//     lv_spangroup_set_overflow(spangroup, LV_SPAN_OVERFLOW_CLIP);                                    /* 设置spangroup溢出 */
//     lv_spangroup_set_indent(spangroup, 20);                                                         /* 设置spangroup缩进 */
//     lv_spangroup_set_mode(spangroup, LV_SPAN_MODE_BREAK);                                           /* 设置spangroup模式 */

//     lv_span_t* span = lv_spangroup_new_span(spangroup);                                             /* 创建并新建span */
//     lv_span_set_text(span, "This is span 1.");                                                      /* 设置span文本 */
//     lv_style_set_text_color(&span->style, lv_palette_main(LV_PALETTE_RED));                         /* 设置span文本颜色 */
//     lv_style_set_text_decor(&span->style, LV_TEXT_DECOR_STRIKETHROUGH | LV_TEXT_DECOR_UNDERLINE);   /* 设置span文本格式 */
//     lv_style_set_text_opa(&span->style, LV_OPA_30);                                                 /* 设置span文本透明度 */

//     span = lv_spangroup_new_span(spangroup);                                                        /* 新建span */
//     lv_span_set_text_static(span, "This is span 2.");                                               /* 设置span文本 */
//     lv_style_set_text_font(&span->style, &lv_font_montserrat_16);                                   /* 设置span文本字体 */
//     lv_style_set_text_color(&span->style, lv_palette_main(LV_PALETTE_GREEN));                       /* 设置span文本颜色 */
                                                                                                       
//     span = lv_spangroup_new_span(spangroup);                                                        /* 新建span */
//     lv_span_set_text_static(span, "This is span 3.");                                               /* 设置span文本 */
//     lv_style_set_text_color(&span->style, lv_palette_main(LV_PALETTE_BLUE));                        /* 设置span文本颜色 */
                                                                                                       
//     span = lv_spangroup_new_span(spangroup);                                                        /* 新建span */
//     lv_span_set_text_static(span, "This is span 4.");                                               /* 设置span文本 */
//     lv_style_set_text_color(&span->style, lv_palette_main(LV_PALETTE_GREEN));                       /* 设置span文本颜色 */
//     lv_style_set_text_font(&span->style, &lv_font_montserrat_14);                                   /* 设置span文本字体 */
//     lv_style_set_text_decor(&span->style, LV_TEXT_DECOR_UNDERLINE);                                 /* 设置span文本格式 */
                                                                                                       
//     span = lv_spangroup_new_span(spangroup);                                                        /* 新建span */
//     lv_span_set_text(span, "This is span 5.");                                                      /* 设置span文本 */
                                                                                                       
//     lv_spangroup_refr_mode(spangroup);                                                              /* 更新spangroup模式 */
// }

// /**
//  * @brief  LVGL演示
//  * @param  无
//  * @return 无
//  */
// void lv_mainstart(void)
// {
//     lv_example_span();
// }
/*------------- SPAN -------------*/

/*------------- 微调器 -------------*/
// static lv_obj_t *spinbox;                          /* 微调器 */
// static lv_obj_t *btn_up;                           /* 递增按钮 */
// static lv_obj_t *btn_down;                         /* 递减按钮 */

// /**
//  * @brief  按钮事件回调
//  * @param  *e ：事件相关参数的集合，它包含了该事件的所有数据
//  * @return 无
//  */
// static void btn_event_cb(lv_event_t *e)
// {
//     lv_obj_t *target = lv_event_get_target(e);     /* 获取触发源 */

//     if (target == btn_up)                          /* 递增按钮按下 */
//     {
//         lv_spinbox_increment(spinbox);             /* 数值递增 */
//     }
//     else if (target == btn_down)                   /* 递减按钮按下 */
//     {
//         lv_spinbox_decrement(spinbox);             /* 数值递减 */
//     }
// }

// /**
//  * @brief  微调器事件回调
//  * @param  *e ：事件相关参数的集合，它包含了该事件的所有数据
//  * @return 无
//  */
// static void spinbox_event_cb(lv_event_t *e)
// {
//     float spinbox_value;                           /* 微调器当前值 */

//     lv_event_code_t code = lv_event_get_code(e);   /* 获取事件类型 */
    
//     if(code == LV_EVENT_VALUE_CHANGED)
//     {
//         spinbox_value= (float)lv_spinbox_get_value(spinbox);

//         printf("%.1f\r\n", spinbox_value/10);
//     }
// }

// /**
//  * @brief  微调器实例
//  * @param  无
//  * @return 无
//  */
// static void lv_example_spinbox(void)
// {
//     /* 微调器 */
//     spinbox = lv_spinbox_create(lv_scr_act());                                          /* 创建微调器 */
// 	lv_spinbox_set_step(spinbox,2);
//     lv_spinbox_set_range(spinbox, -10000, 10000);                                       /* 设置范围值 */
//     lv_spinbox_set_digit_format(spinbox, 5, 4);                                         /* 设置数字格式 */
//     lv_obj_set_size(spinbox, 150, 47);                                                  /* 设置大小 */
//     lv_obj_center(spinbox);                                                             /* 设置位置 */
//     lv_obj_update_layout(spinbox);                                                      /* 更新布局 */
//     lv_obj_set_style_text_font(spinbox, &lv_font_montserrat_14, LV_PART_MAIN);          /* 设置字体 */
//     lv_obj_set_style_text_align(spinbox, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);           /* 设置文本对齐方式 */
//     lv_obj_add_event_cb(spinbox, spinbox_event_cb, LV_EVENT_VALUE_CHANGED, NULL);       /* 添加事件回调 */

//     /* 递增按钮 */
//     btn_up = lv_btn_create(lv_scr_act());                                               /* 创建按钮 */
//     lv_obj_set_size(btn_up, 38, 38);                                                    /* 设置大小 */
//     lv_obj_align_to(btn_up, spinbox, LV_ALIGN_OUT_LEFT_MID, -10, 0);                    /* 设置位置 */
//     lv_obj_set_style_bg_img_src(btn_up, LV_SYMBOL_PLUS, LV_PART_MAIN);                  /* 设置背景图标 */
//     lv_obj_add_event_cb(btn_up, btn_event_cb, LV_EVENT_CLICKED, NULL);                  /* 添加事件回调 */

//     /* 递减按钮 */
//     btn_down = lv_btn_create(lv_scr_act());                                             /* 创建按钮 */
//     lv_obj_set_size(btn_down, 38, 38);                                                  /* 设置大小 */
//     lv_obj_align_to(btn_down, spinbox, LV_ALIGN_OUT_RIGHT_MID, 10, 0);                  /* 设置位置 */
//     lv_obj_set_style_bg_img_src(btn_down, LV_SYMBOL_MINUS, LV_PART_MAIN);               /* 设置背景图标 */
//     lv_obj_add_event_cb(btn_down, btn_event_cb, LV_EVENT_CLICKED, NULL);                /* 添加事件回调 */
// }

// /**
//  * @brief  LVGL演示
//  * @param  无
//  * @return 无
//  */
// void lv_mainstart(void)
// {
//     lv_example_spinbox();
// }
/*------------- 微调器 -------------*/

/*------------- 加载器 -------------*/
// /* 获取当前活动屏幕的宽高 */
// #define scr_act_width() lv_obj_get_width(lv_scr_act())
// #define scr_act_height() lv_obj_get_height(lv_scr_act())

// static const lv_font_t *font;                   /* 定义字体 */
// static lv_obj_t *spinner;                       /* 加载器 */
// static lv_obj_t *label_load;                    /* 加载标题标签 */

// /**
//  * @brief  加载提示标签
//  * @param  无
//  * @return 无
//  */
// static void lv_example_label(void)
// {
//     /* 根据活动屏幕宽度选择字体 */
//     if (scr_act_width() <= 480)
//     {
//         font = &lv_font_montserrat_14;
//     }
//     else
//     {
//         font = &lv_font_montserrat_20;
//     }
    
//     /* 加载标题标签 */
//     label_load = lv_label_create(lv_scr_act());
//     lv_label_set_text(label_load, "LOADING...");
//     lv_obj_set_style_text_font(label_load, font, LV_STATE_DEFAULT);
//     lv_obj_align(label_load, LV_ALIGN_CENTER, 0, scr_act_height() / 10 );
// }

// /**
//  * @brief  加载器显示
//  * @param  无
//  * @return 无
//  */
// static void lv_example_spinner(void)
// {
//     spinner = lv_spinner_create(lv_scr_act(), 1000, 60);                            /* 创建加载器 */
//     lv_obj_align(spinner, LV_ALIGN_CENTER, 0, -scr_act_height() / 15 );             /* 设置位置 */
//     lv_obj_set_size(spinner, scr_act_height() / 5, scr_act_height() / 5);           /* 设置大小 */
//     lv_obj_set_style_arc_width(spinner, scr_act_height() / 35, LV_PART_MAIN);       /* 设置主体圆弧宽度 */
//     lv_obj_set_style_arc_width(spinner, scr_act_height() / 35, LV_PART_INDICATOR);  /* 设置指示器圆弧宽度 */
// }

// /**
//  * @brief  LVGL演示
//  * @param  无
//  * @return 无
//  */
// void lv_mainstart(void)
// {
//     lv_example_label();            /* 加载提示标签 */
//     lv_example_spinner();          /* 加载器显示 */
// }

/*------------- 加载器 -------------*/

/*------------- 选项卡 -------------*/
// /* 获取当前活动屏幕的宽高 */
// #define scr_act_width()  lv_obj_get_width(lv_scr_act())
// #define scr_act_height() lv_obj_get_height(lv_scr_act())

// static const lv_font_t *font;           /* 定义字体 */

// /**
//  * @brief  选项卡实例
//  * @param  无
//  * @return 无
//  */
// static void lv_example_tabview(void)
// {
//     /* 根据屏幕大小设置字体 */
//     if (scr_act_width() <= 480)
//     {
//         font = &lv_font_montserrat_14;
//     }
//     else
//     {
//         font = &lv_font_montserrat_20;
//     }

//     /*************************   第一部分：选项卡（基础）   ****************************/

//     lv_obj_t *tabview = lv_tabview_create(lv_scr_act(), LV_DIR_TOP, scr_act_height()/6);        /* 创建选项卡 */
//     lv_obj_set_style_text_font(tabview, font, LV_STATE_DEFAULT);                                /* 设置字体 */

//     lv_obj_t *tab1 = lv_tabview_add_tab(tabview, "Message");                                    /* 添加选项卡1 */
//     lv_obj_t *tab2 = lv_tabview_add_tab(tabview, "Schedule");                                   /* 添加选项卡2 */
//     lv_obj_t *tab3 = lv_tabview_add_tab(tabview, "Meeting");                                    /* 添加选项卡3 */

//     lv_obj_t *label1 = lv_label_create(tab1);                                                   /* 创建标签（在选项卡1内） */
//     lv_label_set_text(label1, "Tonight's meeting cancelled.");                                  /* 设置文本内容 */
//     lv_obj_center(label1);                                                                      /* 设置位置 */

//     lv_obj_t *label2 = lv_label_create(tab2);                                                   /* 创建标签（在选项卡2内） */
//     lv_label_set_text(label2, "AM   8:30   meet the client\n\n" "PM   13:30   factory tour");   /* 设置文本内容 */
//     lv_obj_center(label2);                                                                      /* 设置位置 */

//     lv_obj_t *label3 = lv_label_create(tab3);                                                   /* 创建标签（在选项卡3内） */
//     lv_label_set_text(label3, "None");                                                          /* 设置文本内容 */
//     lv_obj_center(label3);                                                                      /* 设置位置 */

//     /*************************   第二部分：选项卡（界面优化）   ************************/

//     /* 1、按钮 */
//     lv_obj_t *btn = lv_tabview_get_tab_btns(tabview);                                           /* 获取按钮部分 */

//     /* 未选中的按钮 */
//     lv_obj_set_style_bg_color(btn, lv_color_hex(0xb7472a), LV_PART_ITEMS|LV_STATE_DEFAULT);     /* 设置按钮背景颜色：橙色 */
//     lv_obj_set_style_bg_opa(btn, 200, LV_PART_ITEMS|LV_STATE_DEFAULT);                          /* 设置按钮背景透明度 */
//     lv_obj_set_style_text_color(btn, lv_color_hex(0xf3f3f3), LV_PART_ITEMS|LV_STATE_DEFAULT);   /* 设置按钮文本颜色：白色 */

//     /* 选中的按钮 */
//     lv_obj_set_style_bg_color(btn, lv_color_hex(0xe1e1e1), LV_PART_ITEMS|LV_STATE_CHECKED);     /* 设置按钮背景颜色：灰色 */
//     lv_obj_set_style_bg_opa(btn, 200, LV_PART_ITEMS|LV_STATE_CHECKED);                          /* 设置按钮背景透明度 */
//     lv_obj_set_style_text_color(btn, lv_color_hex(0xb7472a), LV_PART_ITEMS|LV_STATE_CHECKED);   /* 设置按钮文本颜色：橙色 */
//     lv_obj_set_style_border_width(btn, 0, LV_PART_ITEMS| LV_STATE_CHECKED);                     /* 设置按钮边框宽度为0 */

//     /* 2、主体 */
//     lv_obj_t *obj = lv_tabview_get_content(tabview);                                            /* 获取主体部分 */
//     lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffff), LV_STATE_DEFAULT);                   /* 设置背景颜色：白色 */
//     lv_obj_set_style_bg_opa(obj, 255, LV_STATE_DEFAULT);                                        /* 设置背景透明度 */
// }

// /**
//  * @brief  LVGL演示
//  * @param  无
//  * @return 无
//  */
// void lv_mainstart(void)
// {
//     lv_example_tabview();
// }
/*------------- 选项卡 -------------*/

/*------------- 平铺视图 -------------*/
// /* 获取当前活动屏幕的宽高 */
// #define scr_act_width()  lv_obj_get_width(lv_scr_act())
// #define scr_act_height() lv_obj_get_height(lv_scr_act())

// static const lv_font_t *font;           /* 定义字体 */

// /**
//  * @brief  平铺视图实例
//  * @param  无
//  * @return 无
//  */
// static void lv_example_tileview(void)
// {
//     /* 根据屏幕宽度设置字体 */
//     if (scr_act_width() <= 480)
//     {
//         font = &lv_font_montserrat_14;
//     }
//     else
//     {
//         font = &lv_font_montserrat_20;
//     }

//     /* 创建平铺视图页面 */
//     lv_obj_t *tileview = lv_tileview_create(lv_scr_act());                                  /* 创建平铺视图 */

//     lv_obj_t *tile_1 = lv_tileview_add_tile( tileview, 0, 0, LV_DIR_RIGHT );                /* 添加页面1 */
//     lv_obj_t *tile_2 = lv_tileview_add_tile( tileview, 1, 0, LV_DIR_LEFT|LV_DIR_RIGHT );    /* 添加页面2 */
//     lv_obj_t *tile_3 = lv_tileview_add_tile( tileview, 2, 0, LV_DIR_LEFT );                 /* 添加页面3 */

//     /* 设置页面内容 */
//     lv_obj_t *label_1 = lv_label_create(tile_1);                                            /* 创建标签 */
//     lv_label_set_text(label_1, "You");                                                   /* 设置文本内容 */
//     lv_obj_set_style_text_font(label_1, font, LV_STATE_DEFAULT);                            /* 设置字体 */
//     lv_obj_center(label_1);                                                                 /* 设置位置 */

//     lv_obj_t *label_2 = lv_label_create(tile_2);                                            /* 创建标签 */
//     lv_label_set_text(label_2, "Are");                                                   /* 设置文本内容 */
//     lv_obj_set_style_text_font(label_2, font, LV_STATE_DEFAULT);                            /* 设置字体 */
//     lv_obj_center(label_2);                                                                 /* 设置位置 */

//     lv_obj_t *label_3 = lv_label_create(tile_3);                                            /* 创建标签 */
//     lv_label_set_text(label_3, "Dog");                                                   /* 设置文本内容 */
//     lv_obj_set_style_text_font(label_3, font, LV_STATE_DEFAULT);                            /* 设置字体 */
//     lv_obj_center(label_3);                                                                 /* 设置位置 */
//     lv_obj_remove_style(tileview, NULL, LV_PART_SCROLLBAR);                                 /* 移除滚动条 */

//     /* 左侧状态栏 */
//     lv_obj_t *label_left = lv_label_create(lv_scr_act());                                   /* 创建标签 */
//     lv_label_set_text(label_left, "AM 8:30" );                                              /* 设置文本内容 */
//     lv_obj_set_style_text_font(label_left, font, LV_STATE_DEFAULT);                         /* 设置字体 */
//     lv_obj_align(label_left, LV_ALIGN_TOP_LEFT, 10, 10);                                    /* 设置位置 */

//     /* 右侧状态栏 */
//     lv_obj_t *label_right = lv_label_create(lv_scr_act());                                  /* 创建标签 */
//     lv_label_set_text(label_right, LV_SYMBOL_WIFI "   80% " LV_SYMBOL_BATTERY_3);           /* 设置文本内容 */
//     lv_obj_set_style_text_font(label_right, font, LV_STATE_DEFAULT);                        /* 设置字体 */
//     lv_obj_align(label_right, LV_ALIGN_TOP_RIGHT, -10, 10);                                 /* 设置位置 */
// }

// /**
//  * @brief  LVGL演示
//  * @param  无
//  * @return 无
//  */
// void lv_mainstart(void)
// {
//     lv_example_tileview();
// }
/*------------- 平铺视图 -------------*/

/*------------- 弹窗 -------------*/
/* 获取当前活动屏幕的宽高 */
#define scr_act_width()  lv_obj_get_width(lv_scr_act())
#define scr_act_height() lv_obj_get_height(lv_scr_act())

static const lv_font_t *font;                                       /* 定义字体 */
static lv_obj_t *win;                                               /* 定义窗口 */

/**
 * @brief  按钮事件回调
 * @param  *e ：事件相关参数的集合，它包含了该事件的所有数据
 * @return 无
 */
static void btn_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);                    /* 获取事件类型 */

    if(code == LV_EVENT_CLICKED)                                    /* 按钮按下 */
    {
        lv_obj_add_flag(win, LV_OBJ_FLAG_HIDDEN);                   /* 隐藏窗口 */
    }
}

/**
 * @brief  窗口实例
 * @param  无
 * @return 无
 */
static void lv_example_win(void)
{
    /* 根据屏幕宽度选择字体 */
    if (scr_act_width() <= 480)
    {
        font = &lv_font_montserrat_12;
    }
    else
    {
        font = &lv_font_montserrat_20;
    }

    /* 窗口整体 */
    win = lv_win_create(lv_scr_act(), scr_act_height()/12);                                     /* 创建窗口 */
    lv_obj_set_size(win, scr_act_width()* 5/8, scr_act_height()* 4/7);                          /* 设置大小 */
    lv_obj_center(win);                                                                         /* 设置位置 */
    lv_obj_set_style_border_width(win, 1, LV_STATE_DEFAULT);                                    /* 设置边框宽度 */
    lv_obj_set_style_border_color(win, lv_color_hex(0x8a8a8a), LV_STATE_DEFAULT);               /* 设置边框颜色 */
    lv_obj_set_style_border_opa(win, 100, LV_STATE_DEFAULT);                                    /* 设置边框透明度 */
    lv_obj_set_style_radius(win, 10, LV_STATE_DEFAULT);                                         /* 设置圆角 */

    /*************************   第一部分：头部   ****************************/

    /* 左侧按钮 */
    lv_obj_t *btn_setting = lv_win_add_btn(win, LV_SYMBOL_SETTINGS,30);                         /* 添加按钮 */
    lv_obj_set_style_bg_opa(btn_setting, 0, LV_STATE_DEFAULT);                                  /* 设置背景透明度 */
    lv_obj_set_style_shadow_width(btn_setting, 0, LV_STATE_DEFAULT);                            /* 设置阴影宽度 */
    lv_obj_set_style_text_color(btn_setting, lv_color_hex(0x000000), LV_STATE_DEFAULT);         /* 设置文本颜色 */

    /* 标题 */
    lv_obj_t *title = lv_win_add_title(win, "Setting");                                         /* 添加标题 */
    lv_obj_set_style_text_font(title, font, LV_STATE_DEFAULT);                                  /* 设置字体 */

    /* 右侧按钮 */
    lv_obj_t *btn_close = lv_win_add_btn(win, LV_SYMBOL_CLOSE,30);                              /* 添加按钮 */
    lv_obj_set_style_bg_opa(btn_close, 0, LV_STATE_DEFAULT);                                    /* 设置背景透明度 */
    lv_obj_set_style_shadow_width(btn_close, 0, LV_STATE_DEFAULT);                              /* 设置阴影宽度 */
    lv_obj_set_style_text_color(btn_close, lv_color_hex(0x000000), LV_STATE_DEFAULT);           /* 设置文本颜色（未按下） */
    lv_obj_set_style_text_color(btn_close, lv_color_hex(0xff0000), LV_STATE_PRESSED);           /* 设置文本颜色（已按下） */
    lv_obj_add_event_cb(btn_close, btn_event_cb, LV_EVENT_CLICKED, NULL);                       /* 添加事件 */

    /*************************   第二部分：主体   ****************************/

    /* 主体背景 */
    lv_obj_t *content = lv_win_get_content(win);                                                /* 获取主体 */
    lv_obj_set_style_bg_color(content, lv_color_hex(0xffffff),LV_STATE_DEFAULT);                /* 设置背景颜色 */

    /* 音乐音量滑块 */
    lv_obj_t *slider_audio = lv_slider_create(content);                                         /* 创建滑块 */
    lv_obj_set_size(slider_audio, scr_act_width()/3, scr_act_height()/30);                      /* 设置大小 */
    lv_obj_align(slider_audio, LV_ALIGN_CENTER, 15, -scr_act_height()/14);                      /* 设置位置 */
    lv_slider_set_value(slider_audio, 50, LV_ANIM_OFF);                                         /* 设置当前值 */
    lv_obj_set_style_bg_color(slider_audio, lv_color_hex(0x787c78), LV_PART_MAIN);              /* 设置主体颜色 */
    lv_obj_set_style_bg_color(slider_audio, lv_color_hex(0xc3c3c3), LV_PART_INDICATOR);         /* 设置指示器颜色 */
    lv_obj_remove_style(slider_audio, NULL, LV_PART_KNOB);                                      /* 移除旋钮 */

    /* 音乐音量图标 */
    lv_obj_t *label_audio = lv_label_create(content);                                           /* 创建音量标签 */
    lv_label_set_text(label_audio, LV_SYMBOL_AUDIO);                                            /* 设置文本内容：音乐图标 */
    lv_obj_set_style_text_font(label_audio, font, LV_STATE_DEFAULT);                            /* 设置字体 */
    lv_obj_align_to(label_audio, slider_audio, LV_ALIGN_OUT_LEFT_MID, -scr_act_width()/40, 0);  /* 设置位置 */

    /* 闹钟音量滑块 */
    lv_obj_t *slider_bell = lv_slider_create(content);                                          /* 创建滑块 */
    lv_obj_set_size(slider_bell, scr_act_width()/3, scr_act_height()/30);                       /* 设置大小 */
    lv_obj_align(slider_bell, LV_ALIGN_CENTER, 15, scr_act_height()/14);                        /* 设置位置 */
    lv_slider_set_value(slider_bell, 50, LV_ANIM_OFF);                                          /* 设置当前值 */
    lv_obj_set_style_bg_color(slider_bell, lv_color_hex(0x787c78), LV_PART_MAIN);               /* 设置主体颜色 */
    lv_obj_set_style_bg_color(slider_bell, lv_color_hex(0xc3c3c3), LV_PART_INDICATOR);          /* 设置指示器颜色 */
    lv_obj_remove_style(slider_bell, NULL, LV_PART_KNOB);                                       /* 移除旋钮 */

    /* 闹钟音量图标 */
    lv_obj_t *label_bell = lv_label_create(content);                                            /* 创建音量标签 */
    lv_label_set_text(label_bell, LV_SYMBOL_BELL);                                              /* 设置文本内容：闹钟图标 */
    lv_obj_set_style_text_font(label_bell, font, LV_STATE_DEFAULT);                             /* 设置字体 */
    lv_obj_align_to(label_bell, slider_bell, LV_ALIGN_OUT_LEFT_MID, -scr_act_width()/40, 0);    /* 设置位置 */
}

/**
 * @brief  LVGL演示
 * @param  无
 * @return 无
 */
void lv_mainstart(void)
{
    lv_example_win();
}

/*------------- 弹窗 -------------*/


