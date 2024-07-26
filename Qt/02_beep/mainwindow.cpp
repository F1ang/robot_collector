#include "mainwindow.h"
#include <QDebug>
#include <QGuiApplication>
#include <QScreen>
#include <QRect>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    /* 获取屏幕的分辨率，Qt官方建议使用这
     * 种方法获取屏幕分辨率，防上多屏设备导致对应不上
     * 注意，这是获取整个桌面系统的分辨率
     */
    QList <QScreen *> list_screen =  QGuiApplication::screens();

    /* 如果是ARM平台，直接设置大小为屏幕的大小 */
#if __arm__
    /* 重设大小 */
    this->resize(list_screen.at(0)->geometry().width(),
                 list_screen.at(0)->geometry().height());
#else
    /* 否则则设置主窗体大小为800x480 */
    this->resize(800, 480);
#endif
    state = false;

    pushButton = new QPushButton(this);

    /* 居中显示 */
    pushButton->setMinimumSize(200, 50);
    pushButton->setGeometry((this->width() - pushButton->width()) /2 ,
                            (this->height() - pushButton->height()) /2,
                            pushButton->width(),
                            pushButton->height()
                            );
    /* 获取BEEP的状态 */
    getBeepState();

    /* 信号槽连接 */
    connect(pushButton, SIGNAL(clicked()),
            this, SLOT(pushButtonClicked()));
}


MainWindow::~MainWindow()
{
}

void MainWindow::setBeepState()
{
    char const *filename = "/dev/miscbeep";  // 可执行文件名
    int fd,ret;                             //  fd: 文件句柄 ret:函数操作返回值
    unsigned char databuf[1]; 

    /* 在设置BEEP状态时先读取 */
    getBeepState();

    /* 打开LED文件 */
    fd = open(filename, O_RDWR);  // 可读可写
    if(fd < 0){  // 打开文件错误
       ::close(fd);
    }

    // 判断BEEP的状态
    if(state){
        databuf[0] = 0;
        state = false;
    }
    else{
        databuf[0] = 1;
        state = true;
    }

    /* 向文件中写入数据 */
     ret = write(fd, databuf, sizeof(databuf));
        if(ret < 0){
            ::close(fd);
        }else{ 
     }

    /* 关闭文件 */
    ::close(fd);

    getBeepState();
}

bool MainWindow::getBeepState()
{
    if(state == false){  
        pushButton->setText("开启BEEP");
        return true;
    }else{
        pushButton->setText("关闭BEEP");
        return false;
    }
}

void MainWindow::pushButtonClicked()
{
    /* 设置蜂鸣器的状态 */
    setBeepState();
}


