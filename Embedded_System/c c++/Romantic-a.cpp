/*
 * @Description: 
 * @Author: YiPei_Fang
 * @Date: 2022-04-10 11:14:32
 */
// #include <iostream>
// using namespace std;
// int main()
// {
//     std::cout<< "hello world" <<std::endl;
//     cout<<"welcome to c++ and vscode!"<<'\n';
//     return 0;
// }

/*
   用powershell终端:
   1)g++ Romantic-a.cpp -o Romantic-a.exe; 
   2)执行./Romantic-a.exe 
*/



/**
 * @description:阶乘练习 
 * @return {*}
 * @Author: YiPei_Fang
 * @Date: 2022-10-15 19:25:11
 */
#include <iostream>
#include <limits>
#include <cmath>//数学运算函数
#include <string>//字符串运算
#include <ctime>//时间函数
#include "Romantic-a.h"
using namespace std;

data_re data;

class functions{
    public:
        int x1 = 0;
        int y1 = 0;
        int sum1 = 0;
        int function1(int number);
        int function2(int number);
        int function3(int number);

        void time_t_function(void);
    private:
        int x = 0;
        int y = 0;
        int sum = 0;
};

functions functions_init;
int main(){
    cout << "recursion:" << "\n";
    cin >> functions_init.x1;
    //3)处理递归终止办法
    functions_init.sum1 = functions_init.function1(functions_init.x1);
    printf("sum1=%d\n",functions_init.sum1);

    cout << "fibonacci:"<<"\n";
    cin >> functions_init.x1;
    functions_init.sum1 = functions_init.function2(functions_init.x1); 
    printf("sum1=%d\n",functions_init.sum1);

    cout << "fibonacci_2:"<<"\n";
    cin >> functions_init.x1;
    functions_init.sum1 = functions_init.function3(functions_init.x1); 
    printf("sum1=%d\n",functions_init.sum1);    

    data.F1ang = 1;
    cout << F2ang << endl;
    cout << data.F1ang << endl;

    functions_init.time_t_function();

    return 0;
}

/**
 * @description: 递归算法:阶乘n
 * @return {*}
 * @Author: YiPei_Fang
 * @Date: 2022-10-15 19:38:42
 * @param {int} number
 */
int functions::function1(int number){
    //1)明确递归终止条件
    if(number == 0) return 1;  
    //2)明确递啥,归啥  ps:缩小问题规模
    else return number*function1(number-1);
}

/**
 * @description:递归算法:斐波那契数列(数列n项值)
 * @return {*}
 * @Author: YiPei_Fang
 * @Date: 2022-10-15 20:42:07
 */
int functions::function2(int number){
    //明确递归结束条件
    if(number == 1 || number == 2)  
        return 1;
    else if(number == 0)
        return 0;
    //明确递啥,归啥  ps:缩小问题规模
    else                            
        return (function2(number-1)+function2(number-2));
}

/**
 * @description: 迭代算法:斐波那契数列(数列n项值)
 * @return {*}
 * @Author: YiPei_Fang
 * @Date: 2022-10-15 21:36:04
 */
int functions::function3(int number){
    if(number == 0)
        return 0;
    if(number == 1 || number ==2 )
        return 1;
    int c_1=1,c_2=1,c=0;//前1,前2,和
    
    for(int i=3;i<=number;i++){
        c = c_1+c_2;
        c_2 = c_1;
        c_1 = c;
    }
    return c;
}

void functions::time_t_function(void){
   // 基于当前系统的当前日期/时间
   time_t now = time(0);
   // 把 now 转换为字符串形式
   char* dt = ctime(&now);
   cout << "" << dt << endl;
}



/*
C++学习笔记：
    1)数组,字符数组~字符串->数组名指向数组首地址(数组首元素地址)
    2)结构体:类型,变量,成员变量(struct,于是修饰类型关键词typedef)，
    3)地址指针

    4)类:对象,成员变量,成员函数(区别于方法),继承:父类-基类,子类-派生类,(class,类实例化为对象,public,private,protect)
    5)using namespace std:使用的命名空间,namespace,std,同一空间的规范命名,嵌套
*/


/*
    字符型数组在内存中的储存方式同字符串String类一样，故输出数组名并不是输出数组首地址，而是直接输出数组内容，存在'\0'标志；
    而整形数组只是整形的集合，所以其数组名仅仅是其首地址，我们必须要用for循环才能完整的输出整个整形数组

    数组名指向的就是数组首地址，
    printf("%s",ch);会按照ch指向的地址读取ch长度的数据，读取出来的数据自然是“hello”了

    ps:数组(整形/字符)的关键是  数组名（指向元素首地址），无'\0'的数组则要下标访问，有则直接全部访问。

    类：范围解析运算符'::'，构造函数和析构函数，this指针指向对象自身地址(成员函数的隐含参数)，命名空间(嵌套、变量、函数)
*/