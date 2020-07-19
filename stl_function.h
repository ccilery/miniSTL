#ifndef __STL_FUNCTION_H
#define __STL_FUNCTION_H

// STL函数对象及函数适配器的实现

// 融入STL中的仿函数和函数适配器必须继承自下面两个基类
// 一元函数对象的基类, 提供参数类型和返回值类型供函数适配器使用
template<class Arg, class Result>
struct unary_function
{
    typedef Arg argument_type;
    typedef Result result_type;
};

// 二元函数对象的基类, 提供参数类型和返回值类型供函数适配器使用
template<class Arg1, class Arg2, class Result>
struct binary_function
{
    typedef Arg1 first_argument_type;
    typedef Arg2 second_argument_type;
    typedef Result result_type;
};

// ============================================ 算数类函数对象
template<class T>
struct plus : public binary_function<T, T, T>
{
    T operator()(const T& x, const T& y) const   {   return x + y; }
};

template<class T>
struct minus : public binary_function<T, T, T>
{
    T operator()(const T& x, const T& y) const   {   return x - y; }
};

template<class T>
struct multiplies : public binary_function<T, T, T>
{
    T operator()(const T& x, const T& y) const  {   return x * y; }
};

template<class T>
struct divides : public binary_function<T, T, T>
{
    T operator()(const T& x, const T& y) const    {   return x / y; }
};

template<class T>
struct modulus : public binary_function<T, T, T>
{
    T operator()(const T& x, const T& y) const    {   return x % y; }
};

template<class T>
struct negate : public unary_function<T, T>
{
    T operator()(const T& x) const   {   return -x; }
};

// 某种运算的证同元素, A和该元素做运算, 得到自身
template<class T>
inline T identity_element(plus<T>)  {   return T(0);   }

template<class T>
inline T identity_element(multiplies<T>)  {   return T(1);   }


// ============================================ 关系类函数对象
template <class T>
struct equal_to : public binary_function<T, T, bool> 
{
    bool operator()(const T& x, const T& y) const { return x == y; }
};

template <class T>
struct not_equal_to : public binary_function<T, T, bool> 
{
    bool operator()(const T& x, const T& y) const { return x != y; }
};

template <class T>
struct greater : public binary_function<T, T, bool> 
{
    bool operator()(const T& x, const T& y) const { return x > y; }
};

template <class T>
struct less : public binary_function<T, T, bool> 
{
    bool operator()(const T& x, const T& y) const { return x < y; }
};

template <class T>
struct greater_equal : public binary_function<T, T, bool> 
{
    bool operator()(const T& x, const T& y) const { return x >= y; }
};

template <class T>
struct less_equal : public binary_function<T, T, bool> 
{
    bool operator()(const T& x, const T& y) const { return x <= y; }
};


// ============================================ 逻辑运算类函数对象
template <class T>
struct logical_and : public binary_function<T, T, bool> 
{
    bool operator()(const T& x, const T& y) const { return x && y; }
};

template <class T>
struct logical_or : public binary_function<T, T, bool> 
{
    bool operator()(const T& x, const T& y) const { return x || y; }
};

template <class T>
struct logical_not : public unary_function<T, bool> 
{
    bool operator()(const T& x) const { return !x; }
};

// =========================================== SGI STL 额外补充
template<class T>
struct identity : public unary_function<T, T>
{
    const T& operator()(const T& x) const   {   return x;   }
};

template<class Pair>
struct select1st : public unary_function<Pair, typename Pair::first_type>
{
    const typename Pair::first_type& operator()(const Pair& x) const
    {
        return x.first;
    }
};

template<class Pair>
struct select2nd : public unary_function<Pair, typename Pair::second_type>
{
    const typename Pair::second_type& operator()(const Pair& x) const
    {
        return x.second;
    }
};

template<class Arg1, class Arg2>
struct project1st : public binary_function<Arg1, Arg2, Arg1>
{
    Arg1 operator()(const Arg1& x, const Arg2& y) const     {   return x;   }
};

template<class Arg1, class Arg2>
struct project2nd : public binary_function<Arg1, Arg2, Arg2>
{
    Arg2 operator()(const Arg1& x, const Arg2& y) const    {   return y;   }
};


// ============================ 对返回值进行逻辑否定的函数适配器  not1, not2
// 原来函数对象的返回值是bool类型

// 一元
template<class Predicate>   // 修饰的函数对象的类型, 继承是为了能够再次被适配
class unary_negate : public unary_function<typename Predicate::argument_type, bool>
{
protected:
    Predicate pred;
public:
    explicit unary_negate(const Predicate& x) : pred(x) {}
    bool operator()(const typename Predicate::argument_type& x) const {   return !pred(x);    }
};

template<class Predicate>
inline unary_negate<Predicate> not1(const Predicate& x)
{
    return unary_negate<Predicate>(x);      // 返回一个容器适配器对象
}

// 二元
template<class Predicate>   // 修饰的函数对象的类型
class binary_negate : public binary_function<typename Predicate::first_argument_type, 
                                            typename Predicate::second_argument_type, bool>
{
protected:
    Predicate pred;
public:
    binary_negate(const Predicate& x) : pred(x) {}
    bool operator()(const typename Predicate::first_argument_type& x, const typename Predicate::second_argument_type& y) const
    {
        return !pred(x, y);
    }
};

template<class Predicate>
inline binary_negate<Predicate> not2(const Predicate& x)
{
    return binary_negate<Predicate>(x);
}


// ============================ 对参数进行绑定 bind1st, bind2nd
template<class Operation>
class binder1st : binary_function<typename Operation::first_argument_type, typename Operation::second_argument_type,
                                typename Operation::result_type>
{
protected:
    Operation op;
    typename Operation::first_argument_type arg1;

public:
    explicit binder1st(const Operation& x, const typename Operation::first_argument_type& y) : op(x), arg1(y)  {}
    typename Operation::result_type operator()(const typename Operation::second_argument_type& x) const
    {
        return op(arg1, x);
    }
};

template<class Operation, class T>
inline binder1st<Operation> bind1st(const Operation& op, const T& x)
{   
    typedef typename Operation::first_argument_type first_argument_type;
    return binder1st<Operation>(op, first_argument_type(x));    // 进行类型转换
}

template<class Operation>
class binder2nd : binary_function<typename Operation::first_argument_type, typename Operation::second_argument_type,
                                typename Operation::result_type>
{
protected:
    Operation op;
    typename Operation::second_argument_type arg2;

public:
    explicit binder2nd(const Operation& x, const typename Operation::second_argument_type& y) : op(x), arg2(y)  {}
    typename Operation::result_type operator()(const typename Operation::first_argument_type& x) const
    {
        return op(x, arg2);
    }
};

template<class Operation, class T>
inline binder2nd<Operation> bind2nd(const Operation& op, const T& x)
{
    typedef typename Operation::second_argument_type second_argument_type;
    return binder2nd<Operation>(op, second_argument_type(x));
}

// ============================ 用于函数复合 compose1, compose2
// compose1  h = f(g(x))
template<class Operation1, class Operation2>
class unary_compose : public unary_function<typename Operation2::argument_type, typename Operation1::result_type>
{
protected:
    Operation1 f;
    Operation2 g;
public:
    unary_compose(const Operation1& _f, const Operation2& _g) : f(_f), g(_g)    {}
    typename Operation1::result_type operator()(const typename Operation2::argument_type& x) const 
    {
        return f(g(x));
    }
};

template<class Operation1, class Operation2>
inline unary_compose<Operation1, Operation2> compose1(const Operation1& f, const Operation2& g)
{
    return unary_compose<Operation1, Operation2>(f, g);
}

// compose2 h = f(g1(x), g2(x))
template<class Operation1, class Operation2, class Operation3>
class binary_compose : public unary_function<typename Operation2::argument_type, typename Operation1::result_type>
{
protected:
    Operation1 f;
    Operation2 g1;
    Operation3 g2;
public:
    binary_compose(const Operation1& _f, const Operation2& _g1, const Operation3& _g2) : f(_f), g1(_g1), g2(_g2)    {}
    typename Operation1::result_type operator()(const typename Operation2::argument_type& x) const 
    {
        return f(g1(x), g2(x));
    }
};

template<class Operation1, class Operation2, class Operation3>
inline binary_compose<Operation1, Operation2, Operation3> compose2(const Operation1& f, const Operation2& g1, const Operation3& g2)
{
    return binary_compose<Operation1, Operation2, Operation3>(f, g1, g2);
}


// ============================ 用于函数指针
template<class Arg, class Result>
class pointer_to_unary_function : public unary_function<Arg, Result>
{
protected:
    Result (*ptr)(Arg);
public:
    pointer_to_unary_function(Result (*x)(Arg)) : ptr(x) {}
    Result operator()(const Arg& arg) const
    {
        return ptr(arg);
    }
};

template<class Arg, class Result>
inline pointer_to_unary_function<Arg, Result> ptr_fun(Result(*x)(Arg))
{
    return pointer_to_unary_function<Arg, Result>(x);
}

template<class Arg1, class Arg2, class Result>
class pointer_to_binary_function : public binary_function<Arg1, Arg2, Result>
{
protected:
    Result (*ptr)(Arg1, Arg2);
public:
    pointer_to_binary_function(Result (*x)(Arg1, Arg2)) : ptr(x) {}
    Result operator()(const Arg1& arg1, const Arg2& arg2) const
    {
        return ptr(arg1, arg2);
    }
};

template<class Arg1, class Arg2, class Result>
inline pointer_to_binary_function<Arg1, Arg2, Result> ptr_fun(Result(*x)(Arg1, Arg2))
{
    return pointer_to_binary_function<Arg1, Arg2, Result>(x);
}


// ============================ 用于成员函数指针
// 通过对象指针调用不带参数的非const成员函数
template<class S, class T>  // S 成员函数的返回值类型, T 成员函数所属的类类型
class mem_fun_t : public unary_function<T*, S>  
{
public:
    explicit mem_fun_t(S (T::*pf)()) : f(pf)    {}
    S operator()(T* p) const {  return (p->*f)(); }   // 成员函数指针的调用 需要借助类对象的指针或引用
private:
    S (T::*f)();    // pointer to member function
};

// 通过对象指针调用不带参数的const成员函数
template<class S, class T>  // S 成员函数的返回值类型, T 成员函数所属的类类型
class const_mem_fun_t : public unary_function<const T*, S>  
{
public:
    explicit const_mem_fun_t(S (T::*pf)() const) : f(pf)    {}
    S operator()(const T* p) const {  return (p->*f)(); }   // const对象只能调用const成员函数
private:
    S (T::*f)() const;      // pointer to const member function
};

// 通过对象引用调用不带参数的非const成员函数
template<class S, class T>  // S 成员函数的返回值类型, T 成员函数所属的类类型
class mem_fun_ref_t : public unary_function<T, S>  
{
public:
    explicit mem_fun_ref_t(S (T::*pf)()) : f(pf)    {}
    S operator()(T& p) const {  return (p.*f)(); }   // 成员函数指针的调用 需要借助类对象的指针或引用
private:
    S (T::*f)();    // pointer to member function
};

// 通过对象引用调用不带参数的const成员函数
template<class S, class T>  // S 成员函数的返回值类型, T 成员函数所属的类类型
class const_mem_fun_ref_t : public unary_function<T, S>  
{
public:
    explicit const_mem_fun_ref_t(S (T::*pf)() const) : f(pf)    {}
    S operator()(const T& p) const {  return (p.*f)(); }   // const对象只能调用const成员函数
private:
    S (T::*f)() const;      // pointer to const member function
};


// 通过对象指针调用带1个参数的非const成员函数
template<class S, class T, class A>  // S 成员函数的返回值类型, T 成员函数所属的类类型, A 成员函数的参数类型
class mem_fun1_t : public binary_function<T*, A, S>  
{
public:
    explicit mem_fun1_t(S (T::*pf)(A)) : f(pf)    {}
    S operator()(T* p, const A& x) const {  return (p->*f)(x); }   // 成员函数指针的调用 需要借助类对象的指针或引用
private:
    S (T::*f)(A);    // pointer to member function
};

// 通过对象指针调用带1个参数的const成员函数
template<class S, class T, class A>  // S 成员函数的返回值类型, T 成员函数所属的类类型, A 成员函数的参数类型
class const_mem_fun1_t : public binary_function<const T*, A, S>   
{
public:
    explicit const_mem_fun1_t(S (T::*pf)(A) const) : f(pf)    {}
    S operator()(const T* p, const A& x) const {  return (p->*f)(x); }   // const对象只能调用const成员函数
private:
    S (T::*f)(A) const;      // pointer to const member function
};

// 通过对象引用调用带1个参数的非const成员函数
template<class S, class T, class A>  // S 成员函数的返回值类型, T 成员函数所属的类类型, A 成员函数的参数类型
class mem_fun1_ref_t : public binary_function<T, A, S> 
{
public:
    explicit mem_fun1_ref_t(S (T::*pf)(A)) : f(pf)    {}
    S operator()(T& p, const A& x) const {  return (p.*f)(x); }   // 成员函数指针的调用 需要借助类对象的指针或引用
private:
    S (T::*f)(A);    // pointer to member function
};

// 通过对象引用调用带1个参数的const成员函数
template<class S, class T, class A>  // S 成员函数的返回值类型, T 成员函数所属的类类型, A 成员函数的参数类型
class const_mem_fun1_ref_t : public binary_function<T, A, S>
{
public:
    explicit const_mem_fun1_ref_t(S (T::*pf)(A) const) : f(pf)    {}
    S operator()(const T& p, const A& x) const {  return (p.*f)(x); }   // const对象只能调用const成员函数
private:
    S (T::*f)(A) const;      // pointer to const member function
};

// helper
template<class S, class T>
inline mem_fun_t<S, T> mem_fun(S (T::*pf)())
{
    return mem_fun_t<S, T>(pf);
}

template<class S, class T>
inline const_mem_fun_t<S, T> mem_fun(S (T::*pf)() const)
{
    return const_mem_fun_t<S, T>(pf);
}

template<class S, class T>
inline mem_fun_ref_t<S, T> mem_fun_ref(S (T::*pf)())
{
    return mem_fun_ref_t<S, T>(pf);
}

template<class S, class T>
inline const_mem_fun_ref_t<S, T> mem_fun_ref(S (T::*pf)() const)
{
    return const_mem_fun_ref_t<S, T>(pf);
}

template<class S, class T, class A>
inline mem_fun1_t<S, T, A> mem_fun1(S (T::*pf)(A))
{
    return mem_fun1_t<S, T, A>(pf);
}

template<class S, class T, class A>
inline const_mem_fun1_t<S, T, A> mem_fun1(S (T::*pf)(A) const)
{
    return const_mem_fun1_t<S, T, A>(pf);
}

template<class S, class T, class A>
inline mem_fun1_ref_t<S, T, A> mem_fun1_ref(S (T::*pf)(A))
{
    return mem_fun1_ref_t<S, T, A>(pf);
}

template<class S, class T, class A>
inline const_mem_fun1_ref_t<S, T, A> mem_fun1_ref(S (T::*pf)(A) const)
{
    return const_mem_fun1_ref_t<S, T, A>(pf);
}
#endif // __STL_FUNCTION_H