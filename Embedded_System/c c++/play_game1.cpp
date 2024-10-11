/*
 * @Description: c++基础
 * @LastEditTime: 2024-09-16 10:58:31
 */
#include <iostream>
#include <cmath>//数学运算函数
#include <string>//字符串运算
#include <ctime>//时间函数

using namespace std;


int main()
{
    int count = 4;
    int var1[3] = {1,2,3};              /* 整形数组 */
    char var2[5]={'a','b','c','d'};     /* 字符型数组 */
    int *num;
    int *num1;
    const char *num2 = "c++";
    string num3[2] = {
        "come on",
        "fantastic",
    };
    /* 字符数组~字符串->数组名=首元素地址 */
    char num4[] = "of_silence";
    char num5[] = {'o','f','_','s','i','l','e','n','c','e','\0'};
    int *ptr1,**ptr2;

    ptr1 = &count;  /* 地址指针 */
    ptr2 = &ptr1;

    cout << *ptr1 << "\n";
    cout << **ptr2 << "\n";

    ptr1 = var1;/* 整形数组 */
    cout << ptr1[1] << endl;

    cout << num4 << endl;                   /* 字符串型数组 */
    cout << num5 << endl;
    cout << &num5[0] << endl;
    cout << "var1=" << var1 << endl;        /* 整型数组=>首地址 */
    cout << "var2=" << &var2[1] << endl;    /* 字符型数组=>首地址指向(直到'\0') */

    num1 = &count;
    cout << *num1 << endl;
    cout << num2 << endl;
    cout << num3[1] << endl;

    const char *names[4] = {
                            "fangyipei",
                            "helloworld",
                            "yejinliang",
                            "yezhiquan",
    }; 

    for (char i = 0; i < 4; i++)
    {
        cout << var2[i] << endl;
    }
    for (int i = 0; i < 3; i++)
    {
        num = &var1[i];
        cout << *num << endl;
        num++;
    }
    while (count)
    {
        cout<< names[count] << "\n";
        count--;
    }

    return 0;
}