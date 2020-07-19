#ifndef __STL_UNINITIALIZED_H
#define __STL_UNINITIALIZED_H

#include <cstring> // memmove

#include "stl_construct.h"
#include "stl_iterator.h"
#include "type_traits.h"

// uninitialized_fill
// uninitialized_copy
// uninitialized_fill_n
// 根据迭代器指向元素类型的特性派送到最高效的版本

// ========================================= uninitialized_fill_n
template<class ForwardIterator, class size, class T>
ForwardIterator __uninitialized_fill_n_aux(ForwardIterator first, size n, const T& x, __false_type)
{
    ForwardIterator cur = first;
    while(n--)
        construct(&*cur++, x);
    return cur;
}

template<class ForwardIterator, class size, class T>
inline ForwardIterator __uninitialized_fill_n_aux(ForwardIterator first, size n, const T& x, __true_type)
{
    return fill_n(first, n, x);
}

template<class ForwardIterator, class size, class T, class T1>
inline ForwardIterator __uninitialized_fill_n(ForwardIterator first, size n, const T& x, T1*)
{
    typedef typename __type_traits<T1>::is_POD_type is_POD;
    return __uninitialized_fill_n_aux(first, n, x, is_POD());
}

// 根据迭代器指向元素类型的特性派送最高效的版本
template<class ForwardIterator, class size, class T>
inline ForwardIterator uninitialized_fill_n(ForwardIterator first, size n, const T& x)
{
    return __uninitialized_fill_n(first, n, x, value_type(first));
}

// ========================================= uninitialized_fill
template <class ForwardIterator, class T>
inline void
__uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, const T& x, __true_type)
{
    fill(first, last, x);
}

template <class ForwardIterator, class T>
void __uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, const T& x, __false_type)
{
    ForwardIterator cur = first;
    while(cur != last)
        construct(&*cur++, x);
}

template <class ForwardIterator, class T, class T1>
inline void __uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& x, T1*) 
{
    typedef typename __type_traits<T1>::is_POD_type is_POD;
    __uninitialized_fill_aux(first, last, x, is_POD());                 
}

template <class ForwardIterator, class T>
inline void uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& x)
{
    __uninitialized_fill(first, last, x, value_type(first));
}


// ========================================= uninitialized_copy
template <class InputIterator, class ForwardIterator>
inline ForwardIterator 
__uninitialized_copy_aux(InputIterator first, InputIterator last, ForwardIterator result, __true_type) 
{
    return copy(first, last, result);
}

template <class InputIterator, class ForwardIterator>
ForwardIterator
__uninitialized_copy_aux(InputIterator first, InputIterator last, ForwardIterator result, __false_type) 
{
    ForwardIterator cur = result;
    while(first != last)
        construct(&*cur++, *first++);
    return cur;
}

template <class InputIterator, class ForwardIterator, class T>
inline ForwardIterator
__uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result, T*) 
{
    typedef typename __type_traits<T>::is_POD_type is_POD;
    return __uninitialized_copy_aux(first, last, result, is_POD());
}

template <class InputIterator, class ForwardIterator>
inline ForwardIterator uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result) 
{
    return __uninitialized_copy(first, last, result, value_type(result));
}

inline char* uninitialized_copy(const char* first, const char* last, char* result) 
{
    memmove(result, first, last - first);
    return result + (last - first);
}

inline wchar_t* uninitialized_copy(const wchar_t* first, const wchar_t* last, wchar_t* result) 
{
    memmove(result, first, sizeof(wchar_t) * (last - first));
    return result + (last - first);
}

#endif // __STL_UNINITIALIZED_H