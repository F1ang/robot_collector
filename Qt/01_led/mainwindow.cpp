#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

#include "mainwindow.h"
#include <QDebug>
#include <QGuiApplication>
#include <QScreen>
#include <QRect>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    /* 获取屏幕的分辨率 Qt官方建议使用这种方法设置屏幕分辨率
    防止多屏设备导致对应不上，注意，这是获取整个桌面系统的分辨率*/
    QList <QScreen *> list_screen = QGuiApplication::screens();

    /* 如果是ARM平台，直接设置大小为屏幕的大小 */
    #if __arm__
        /* 重设大小 */
        this->resize(list_screen.at(0)->geometry().width(),list_screen.at(0)->geometry().height());
    #else
        /* 否则则设置主窗体大小为 800x480 */
        this->resize(800,480);
    #endif

    state = false;

    /* 实例化按键 */
    pushButton = new QPushButton(this);

    /* 按键居中显示 */
    pushButton->setMinimumSize(200,100);  // 设置按键的大小
    pushButton->setGeometry((this->width() - pushButton->width())/2,
                            (this->height() - pushButton->height())/2,
                            pushButton->width(),pushButton->height()); // 设置按键的大小和位置

    /* 获取Led的状态 */
    getLedState();

    /* 信号槽连接 */
    connect(pushButton, SIGNAL(clicked()),this, SLOT(pushButtonClicked()));

}


MainWindow::~MainWindow()
{
}

/* 设置Led的状态 */
void MainWindow::setLedState()
{
    char const *filename = "/dev/led_dts";  // 可执行文件名
    int fd,ret;  //  fd: 文件句柄 ret:函数操作返回值
    unsigned char databuf[1]; // 缓冲区
    
    /* 在设置状态前先读取状态 */
    // state = getLedState();

    /* 打开LED文件 */
    fd = open(filename, O_RDWR);  // 可读可写
    if(fd < 0){  // 打开文件错误
       ::close(fd);
    }

    // 判断LD的状态
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
        }else{ // 写入成功
     }

    /* 关闭文件 */
    ::close(fd);

    /* 重新获取led的状态 */
    getLedState();
}

/* 获取LED的状态 */

bool MainWindow::getLedState()
{
    // char const *filename = "/dev/led_dts";  // 可执行文件名
    // int fd,ret;  //  fd: 文件句柄 ret:函数操作返回值
    // char databuf[1]; // 缓冲区

    // /* 打开LED文件 */
    // fd = open(filename, O_RDWR);  // 可读可写
    // if(fd < 0){  // 打开文件错误
    //    ::close(fd);
    // }

    // /* 读取文件所有数据 */
    // ret = read(fd, databuf, sizeof(databuf));
    // if (ret < 0) {  // 读取文件失败
    //     ::close(fd);
    // }

    // /* 打印出读出的值 */
    // qDebug()<<"databuf:" <<databuf <<endl;

    // /* 关闭文件 */
    // ::close(fd);

    // QString irValue = databuf;

    if(state == false){  
        pushButton->setText("开启灯");
        return true;
    }else{
        pushButton->setText("关闭灯");
        return false;
    }
}
/* 按键点击事件 */
void MainWindow::pushButtonClicked()
{
    /* 反转Beep的状态 */
    setLedState();
}

