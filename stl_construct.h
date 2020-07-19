#ifndef __STL_CONSTRUCT_H
#define __STL_CONSTRUCT_H

#include <new>  // placement new
#include "stl_iterator.h"
#include "type_traits.h"

// 内存空间分配后对象的构造
// 内存空间释放前对象的销毁

// construct
// void construct(T1* p, const T2& value)

// destroy
// void destroy(T* pointer)
// void destroy(ForwardIterator first, ForwardIterator last)
// void destroy(char*, char*)  
// void destroy(wchar_t*, wchar_t*) 

// 在分配空间上构造单个对象
template<class T1, class T2>
inline void construct(T1* p, const T2& value)
{
    new(p) T1(value);   // placement new
}

// 调用析构函数
template<class T>
inline void destroy(T* pointer)
{
    pointer->~T();      // 只是销毁构造的对象（可能释放对象中某些成员动态分配的空间）, 还没有回收对象所在的空间
}

template<class ForwardIterator>
inline void __destroy_aux(ForwardIterator first, ForwardIterator last, __true_type)
{
    // trivial destructor, 什么都不做
}

template<class ForwardIterator>
inline void __destroy_aux(ForwardIterator first, ForwardIterator last, __false_type)
{
    // non-trivial destructor
    while(first != last)
        destroy(&*first++);    // inline void destroy(T* pointer)
}

template<class ForwardIterator, class T>
inline void __destroy(ForwardIterator first, ForwardIterator last, T*)
{
    typedef typename __type_traits<T>::has_trivial_destructor trivial_destructor;
    __destroy_aux(first, last, trivial_destructor());
}


template<class ForwardIterator>
inline void destroy(ForwardIterator first, ForwardIterator last)
{
    __destroy(first, last, value_type(first));  // 编译期间, 根据迭代器指向元素的类型派送到不同的实现
}

// 函数模板的全特化版本
inline void destroy(char*, char*)  {}
inline void destroy(wchar_t*, wchar_t*) {}

#endif // __STL_CONSTRUCT_H