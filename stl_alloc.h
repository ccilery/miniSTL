#ifndef __STL_ALLOC_H
#define __STL_ALLOC_H

#include <cstdlib> // malloc
#include <cstring> // memcpy
#include <iostream>

// 内存空间不足, 并且没有设置malloc_handler时采取的动作
#define __THROW_BAD_ALLOC exit(1)


// 容器中使用的类模板, 以元素为单位进行管理, 实现上调用一/二级分配器,转化为以字节为单位进行管理
template<class T, class Alloc>
class simple_alloc
{
public:
    // 静态成员函数, 可以通过类名直接调用
    static T* allocate(size_t n)
    {
        return 0 == n ? NULL : (T*)Alloc::allocate(n * sizeof(T));
    }
    static T* allocate(void)
    {
        return (T*)Alloc::allocate(sizeof(T));
    }
    static void deallocate(T* p, size_t n)
    {
        if(0 != n)
            Alloc::deallocate(p, n * sizeof(T));
    }
    static void deallocate(T* p)
    {
        Alloc::deallocate(p, sizeof(T));
    }
};

// ==============================================  一级分配器
// 模板参数可以是类型模板参数, 也可以是非类型模板参数, 这里没有类型参数, 非类型参数也没有用到
template<int inst>
class __malloc_alloc_template
{
private:
    typedef void (*malloc_handler)();
    // 用来处理内存不足的情况, 不断调用malloc_handler, 直到分配成功
    // oom : out of memory
    // 静态成员函数
    static void *oom_malloc(size_t);
    static void *oom_realloc(void*, size_t);
    // 静态数据成员, 函数指针
    // static void (*__malloc_alloc_oom_handler)();
    static malloc_handler __malloc_alloc_oom_handler;

public:
    static void* allocate(size_t n)
    {
        void *result = malloc(n);   // 直接使用malloc
        if(result == NULL)          // 空间不足
            result = oom_malloc(n);
        return result;
    }
    static void deallocate(void* p, size_t)
    {
        free(p);
    }
    static void* reallocate(void* p, size_t, size_t new_sz)
    {
        void *result = realloc(p, new_sz);
        if(result == NULL)
            result = oom_realloc(p, new_sz);
        return result;
    }
    // 模拟new_hander, set_new_handler, 客户端指定内存不足时的处理例程
    // static void (* set_malloc_handler(void (*f)()))()  看不懂???
    static malloc_handler set_malloc_handler(malloc_handler f)
    {
        malloc_handler old = __malloc_alloc_oom_handler;
        __malloc_alloc_oom_handler = f;
        return old;
    }
};

// 静态数据成员初始化
template<int inst>
typename __malloc_alloc_template<inst>::malloc_handler 
__malloc_alloc_template<inst>::__malloc_alloc_oom_handler = 0;

template<int inst>
void* __malloc_alloc_template<inst>::oom_malloc(size_t n)
{
    void *result;
    void (*my_malloc_handler)();
    while(1)        // 不断调用 malloc_handler, 直到分配成功
    {
        my_malloc_handler = __malloc_alloc_oom_handler;
        if(0 == my_malloc_handler)  __THROW_BAD_ALLOC;
        my_malloc_handler();
        result = malloc(n);
        if(result)  return result;
    } 
}

template<int inst>
void* __malloc_alloc_template<inst>::oom_realloc(void* p, size_t n)
{
    void *result;
    void (*my_malloc_handler)();
    while(1)    // 不断调用 malloc_handler, 直到分配成功
    {
        my_malloc_handler = __malloc_alloc_oom_handler;
        if(0 == my_malloc_handler)  {__THROW_BAD_ALLOC;}
        my_malloc_handler();
        result = realloc(p, n);
        if(result)  return result;
    } 
}

typedef __malloc_alloc_template<0> malloc_alloc;

# ifdef __USE_MALLOC
typedef malloc_alloc alloc;
#endif // __USE_MALLOC


// ==============================================  二级分配器, 默认分配器

static const int __ALIGN = 8;
static const int __MAX_BYTES = 128;
static const int __NFREELISTS = __MAX_BYTES / __ALIGN; // 自由链表的条数

template<bool threads, int inst>
class __default_alloc_template
{
private:
    // 嵌入式指针 embeded pointer, 需要保证分配的区块大于指针的大小
    // 在区块没有分配出去时, 利用区块的前四个字节作为指针, 将各个区块串起来, 
    // 区块分配出去后, 整个区块都可以被客户使用
    struct obj
    {
        obj* next;
    };
private:
    // 将区块大小调整到__ALIGN的倍数
    static size_t ROUND_UP(size_t bytes)
    {
        return (((bytes) + __ALIGN-1) & ~(__ALIGN - 1));
    }
    // 返回区块对应的自由链表的下标
    static size_t FREELIST_INDEX(size_t bytes)
    {
        return (((bytes) + __ALIGN-1)/__ALIGN - 1);
    }
private:
    // 自由链表为空, 填充链表
    static void* refill(size_t n);
    // 分配一大块
    static char* chunk_alloc(size_t size, int &nobjs);

private:
    static obj* free_list[__NFREELISTS];
    static char* start_free;
    static char* end_free;
    static size_t heap_size;
public:
    static void* allocate(size_t n) // n字节
    {
        // 调用一级分配器
        if(n > (size_t) __MAX_BYTES)
            return malloc_alloc::allocate(n);
        
        // 找到所在的自由链表    
        obj** my_free_list = free_list + FREELIST_INDEX(n);
        obj* result = *my_free_list;
        if(result == NULL)
        {
            void *r = refill(ROUND_UP(n));  // 自由链表为空, 填充链表
            return r;
        }
        *my_free_list = result->next;
        return result;
    }

    // 缺陷: 1. allocate使用malloc分配内存, deallocate并没有free掉, 而是将所有分配出去包括释放的区块都抓在手里
    // 2. 没有检查p是否是通过alloc分配器分配出去的, 如果是p是通过malloc分配的, 可能会有问题, 无法处理cookie
    static void deallocate(void* p, size_t n)
    {
        if(n > (size_t)__MAX_BYTES)
        {
            malloc_alloc::deallocate(p, n);
            return;
        }

        // 找到所在的自由链表   
        obj** my_free_list = free_list + FREELIST_INDEX(n);
        obj* q = (obj*)p;
        q->next = *my_free_list;
        *my_free_list = q;
    }
    static void* reallocate(void* p, size_t old_sz, size_t new_sz)
    {
        if(old_sz > (size_t)__MAX_BYTES && new_sz > (size_t)__MAX_BYTES)
            return realloc(p, new_sz);
        if(ROUND_UP(old_sz) == ROUND_UP(new_sz))
            return p;

        void *result;
        size_t copy_sz;
        result = allocate(new_sz);
        copy_sz = new_sz > old_sz ? old_sz : new_sz;
        memcpy(result, p, copy_sz);
        deallocate(p, old_sz);
        return result;
    }
};

// 静态数据成员的定义
template <bool threads, int inst>
char *__default_alloc_template<threads, inst>::start_free = nullptr;

template <bool threads, int inst>
char *__default_alloc_template<threads, inst>::end_free = nullptr;

template <bool threads, int inst>
size_t __default_alloc_template<threads, inst>::heap_size = 0;

template <bool threads, int inst>
typename __default_alloc_template<threads, inst>::obj *
__default_alloc_template<threads, inst>::free_list[__NFREELISTS] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, };


// 从暂备池分配一大块
template <bool threads, int inst>
char* __default_alloc_template<threads, inst>::chunk_alloc(size_t size, int &nobjs)
{
    char *result;
    size_t total_bytes = size * nobjs;
    size_t bytes_left = end_free - start_free;
    if(bytes_left >= total_bytes)   // 暂备池剩余满足需要
    {
        result = start_free;
        start_free += total_bytes;
        return result;
    }
    else if(bytes_left >= size)     // 暂备池不能满足需要, 但至少可以分1块
    {
        nobjs = bytes_left / size;
        total_bytes = size * nobjs;
        result = start_free;
        start_free += total_bytes;
        return result;
    }
    else    // 暂备池1块也分不了  
    {
        // 1. 处理暂备池的碎片, 放置到相应大小的自由链表中
        // 2.1 向系统堆要空间 
        // 2.2 系统堆没有, 向 >size的区块链表要空间

        // 处理暂备池的碎片, 放置到相应大小的自由链表中, 碎片大小一定小于 128字节, 否则上面哪个else if就已经分配了
        if(bytes_left > 0)
        {
            obj* p = (obj*)start_free;
            obj** my_free_list = free_list + FREELIST_INDEX(bytes_left);
            p->next = *my_free_list;
            *my_free_list = p;
            start_free = end_free = nullptr;
        }
        

        // 向系统堆要空间
        size_t bytes_to_get = 2 * total_bytes + ROUND_UP(heap_size >> 4);
        start_free = (char*)malloc(bytes_to_get); 

         // 向 >size的区块链表要空间
        if(start_free == nullptr)  
        {
            for(int i = size + __ALIGN; i <= __MAX_BYTES; i += __ALIGN)
            {
                obj** my_free_list = free_list + FREELIST_INDEX(i);
                if(*my_free_list != nullptr)
                {
                    start_free = (char*)*my_free_list;
                    *my_free_list = (*my_free_list)->next;
                    end_free = start_free + i;
                    return chunk_alloc(size, nobjs);
                }     
            }
            start_free = (char*)malloc_alloc::allocate(bytes_to_get);
        }
        heap_size += bytes_to_get;
        end_free = start_free + bytes_to_get;
        return chunk_alloc(size, nobjs);
    } 
}

// n大小的区块对应的链表为空, 需要填充该链表
// n已经上调至8的倍数
template <bool threads, int inst>
void* __default_alloc_template<threads, inst>::refill(size_t n)
{
    int nobjs = 20;
    char* chunk = chunk_alloc(n, nobjs);    // 向暂备池要空间
    if(1 == nobjs)
        return chunk;
    
    obj** my_free_list = free_list + FREELIST_INDEX(n);
    obj* result = (obj*)chunk;  // 作为返回

    // 将剩下的空间划分成区块串起来
    obj* cur;
    obj* next;
    *my_free_list = next = (obj*)(chunk + n);
    for(int i = 1; i < nobjs; ++i)
    {
        cur = next;
        next = (obj*)((char*)cur + n);
        if(i == nobjs - 1)
            cur->next == nullptr;
        else
            cur->next = next;
    }
    return result;
}


typedef __default_alloc_template<false, 0> alloc;


#endif // __STL_ALLOC_H