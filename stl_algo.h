#ifndef __STL_ALGO_H
#define __STL_ALGO_H

template<class InputIterator, class Function>
Function for_each(InputIterator first, InputIterator last, Function f)
{
    for(; first != last; ++first)
        f(*first);
    return f;
}


#endif // __STL_ALGO_H