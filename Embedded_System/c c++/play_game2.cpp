/*
 * @Description: class
 * @LastEditTime: 2023-08-22 19:50:27
 */
#include <iostream>
#include <cmath>//数学运算函数
#include <string>//字符串运算
#include <ctime>//时间函数

using namespace std;

class F1ang
{
    public:
        F1ang(void);/* 构造函数 */
        void F3ang(int write);
        ~F1ang(void);
    private:
        int F2ang;
};

/**
 * @description: 构造函数
 * @return {*}
 * @LastEditTime: Do not edit
 */
F1ang::F1ang(void)
{
    F2ang = 100;
    cout << "F2ang value is:" << F2ang <<endl;
}

void F1ang::F3ang(int write)
{
    F2ang = write;
}

/**
 * @description: 析构函数
 * @return {*}
 * @LastEditTime: Do not edit
 */
F1ang::~F1ang(void)
{
    F2ang = 200;
    cout << "F2ang value is:" << F2ang <<endl;
}

int main(void)
{
    F1ang classf1ang; /* 构造函数执行 */
    return 0;
}






