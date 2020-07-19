// #include "stl_alloc.h"
// #include "stl_construct.h"
#include "memory.h"
#include "type_traits.h"
#include <cstdio>

// alloc 和 construct 的测试文件, 测试 空间分配、对象创建、对象销毁、空间释放 过程

class Foo
{
public:
    int i;
    char *p;
    Foo(int _i) : p((char*)malloc(4)), i(_i) {}
    ~Foo()  {   free(p);  printf("destructor...\n");  }
};

template<>
struct __type_traits<Foo>
{
    typedef __false_type has_trivial_default_constructor;
    typedef __false_type has_trivial_copy_constructor;
    typedef __false_type has_trivial_assigment_operator;
    typedef __false_type has_trivial_destructor;
    typedef __false_type is_POD_type;
};

int main()
{
    Foo* foos[10] = {nullptr};
    typedef simple_alloc<Foo, alloc> data_alloc;

    for(int i = 0; i < 10; ++i)
    {
        foos[i] = data_alloc::allocate(); // 分配空间
        printf("foos[%d] = %p\n", i, foos[i]);
        construct(foos[i], i);  // 构建对象
        strcpy(foos[i]->p, "hah");
    }

    for(int i = 0; i < 10; ++i)
        printf("%s\n", foos[i]->p);

    destroy(foos[0], foos[0] + 10);    // 销毁对象
    for(int i = 0; i < 10; ++i)
        data_alloc::deallocate(foos[i]);    // 释放空间

    for(int i = 0; i < 10; ++i)
    {
        foos[i] = data_alloc::allocate(); // 分配空间, 直接从池中获取
        printf("foos[%d] = %p\n", i, foos[i]);
    }
}
