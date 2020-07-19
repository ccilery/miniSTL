#ifndef __STL_ITERATOR_H
#define __STL_ITERATOR_H

#include <cstddef>  // ptrdiff_t

// 迭代器类型, 类比type_traits中的 __true_type, __false_type
struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag : public input_iterator_tag {};
struct bidirectional_iterator_tag : public forward_iterator_tag {};
struct random_access_iterator_tag : public bidirectional_iterator_tag {};

// 所有类类型的迭代器都继承自该迭代器类, 避免忘记定义这些内嵌类型
// 类类型的迭代器需要与iterator_traits约定好, 自己必须提供这些内嵌类型
template <class Category, class T, class Distance = ptrdiff_t, class Pointer = T*, class Reference = T&>
struct iterator
{
    typedef Category iterator_category;
    typedef T value_type;
    typedef Distance difference_type;
    typedef Pointer pointer;
    typedef Reference reference;
};


// 泛化版本的 iterator_traits
template<class Iterator>
struct iterator_traits
{
    typedef typename Iterator::iterator_category iterator_category;
    typedef typename Iterator::value_type value_type;
    typedef typename Iterator::difference_type difference_type;
    typedef typename Iterator::pointer pointer;
    typedef typename Iterator::reference reference;
};

// 偏特化const指针版本
template<class T>
struct iterator_traits<const T*>
{
    typedef random_access_iterator_tag iterator_category;
    typedef T value_type;
    typedef ptrdiff_t difference_type;
    typedef const T* pointer;
    typedef const T& reference;
};

// 偏特化非const指针版本
template<class T>
struct iterator_traits<T*>
{
    typedef random_access_iterator_tag iterator_category;
    typedef T value_type;
    typedef ptrdiff_t difference_type;
    typedef T* pointer;
    typedef T& reference;
};

//=================================== 萃取迭代器特性的一些工具函数
// 返回一个迭代器类型的对象
template<class Iterator>
inline typename iterator_traits<Iterator>::iterator_category 
iterator_category(const Iterator&)
{
    return typename iterator_traits<Iterator>::iterator_category();
}

template<class Iterator>
inline typename iterator_traits<Iterator>::difference_type* 
distance_type(const Iterator&)
{
    return static_cast<typename iterator_traits<Iterator>::difference_type*>(0);
}

template<class Iterator>
inline typename iterator_traits<Iterator>::value_type* 
value_type(const Iterator&)
{
    return static_cast<typename iterator_traits<Iterator>::value_type*>(0);
}

//================================= distance 函数
template<class InputIterator>
inline typename iterator_traits<InputIterator>::difference_type
__distance(InputIterator first, InputIterator last, input_iterator_tag)
{
    typename iterator_traits<InputIterator>::difference_type n = 0;
    while(first != last)
    {
        ++n; ++first;
    }
    return n;
}

template<class RandomAccessIterator>
inline typename iterator_traits<RandomAccessIterator>::difference_type
__distance(RandomAccessIterator first, RandomAccessIterator last, random_access_iterator_tag)
{
    return last - first;    
}

// [first, last) 之间的距离
template<class InputIterator>
inline typename iterator_traits<InputIterator>::difference_type
distance(InputIterator first, InputIterator last)
{
    // 根据迭代器类型不同，在编译期进行不同的function dispatch
    return __distance(first, last, iterator_category(first));
}

//================================= advance 函数
template<class InputIterator, class Distance>
inline void __advance(InputIterator& i, Distance n, input_iterator_tag)
{
    if(n > 0)
        while(n--)
            ++i;
    else
        while(n++)
            --i;
}

template<class RandomAccessIterator, class Distance>
inline void __advance(RandomAccessIterator& i, Distance n, RandomAccessIterator)
{
    i += n;
}

// i += n
template<class InputIterator, class Distance>
inline void advance(InputIterator& i, Distance n)
{
    __advance(i, n, iterator_category(i));
}

#endif // __STL_ITERATOR_H