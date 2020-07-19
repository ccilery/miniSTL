#include <iostream>
#include "type_traits.h"

using std::cout;
using std::endl;

class Foo
{

};

// 自己编写自定义类型的type_traits
template<>
struct __type_traits<Foo>
{
    typedef __false_type has_trivial_default_constructor;
    typedef __false_type has_trivial_copy_constructor;
    typedef __false_type has_trivial_assigment_operator;
    typedef __false_type has_trivial_destructor;
    typedef __false_type is_POD_type;
};

void func(__true_type true_type)
{
    cout << "true_type版本..." << endl;
}

void func(__false_type false_type)
{
    cout << "false_type版本..." << endl;
}

int main()
{
    typedef __type_traits<int>::has_trivial_default_constructor trivial_ctor1;   // 类型, 萃取类型的特性
    func(trivial_ctor1());   // 模板参数推导时, 根据对象的类型进行派送

    typedef __type_traits<Foo>::has_trivial_default_constructor trivial_ctor2;   // 类型
    func(trivial_ctor2());   // 模板参数推导时, 根据对象的类型进行派送
}
