#include "stl_function.h"
#include "stl_algo.h"
#include <cstdio>
#include <vector>

// 仿函数 和 函数适配器的测试文件

class shape
{
public:
    virtual void display() = 0;
};

class Rect : public shape
{
public:
    virtual void display()  {   printf("Rect...\n"); }
};

class Circle : public shape
{
public:
    virtual void display()  {   printf("Circle...\n"); }
};

class Square : public Rect
{
public:
    virtual void display()  {   printf("Square...\n"); }
};


int main()
{
    printf("%d\n", plus<int>()(2, 4));
    printf("%d\n", divides<int>()(2, 4));
    printf("%lf\n", divides<double>()(2, 4));


    // 函数适配器的测试 not2
    greater_equal<int> op1; // 函数对象
    binary_negate<greater_equal<int>> op2 = not2(op1);  // 函数适配器对象
    printf("%d\n", op1(10, 12));
    printf("%d\n", op2(10, 12));
    printf("%d\n", greater_equal<int>()(10, 12));           // 匿名函数对象greater_equal<int>()
    printf("%d\n", not2(greater_equal<int>())(10, 12));     // 返回一个函数适配器对象 not2(greater_equal<int>())

    // bind1st, bind2nd
    printf("%d\n", bind1st(plus<int>(), 10)(20));
    printf("%d\n", bind2nd(plus<int>(), 10)(20));

    // compose1, compose2
    printf("%d\n", compose1(negate<int>(), negate<int>())(5));
    printf("%d\n", compose2(plus<int>(), negate<int>(), negate<int>())(5));


    // mem_fun
    std::vector<shape*> V;
    V.push_back(new Rect);
    V.push_back(new Circle);
    V.push_back(new Square);
    V.push_back(new Rect);

    for(int i = 0; i < V.size(); i++)
        V[i]->display();

    for_each(V.begin(), V.end(), mem_fun(&shape::display));
     
}