#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QFile>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    /* 存储bool */
    bool state;

    /* 按钮 */
    QPushButton *pushButton;

    /* 文件 */
    QFile file;

    /* 设置BEEP的状态 */
    void setBeepState();

    /* 获取BEEP的状态 */
    bool getBeepState();

private slots:
    /* 槽函数 */
    void pushButtonClicked();
};
#endif // MAINWINDOW_H
