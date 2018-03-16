#ifndef _ATOMIC_H_INCLUDED
#define _ATOMIC_H_INCLUDED

#ifdef _WIN32
    #include <wtypes.h>

    typedef LONG64  atomic_t;

    #define atomic_add_and_fetch(ptr, value) InterlockedAdd64(ptr, value)
    #define atomic_compare_and_swap(ptr, cmp, exc) InterlockedCompareExchange64(ptr, exc, cmp)
#else
    typedef unsigned long long atomic_t;

    #define atomic_add_and_fetch(ptr, value) __sync_add_and_fetch(ptr, value)
    #define atomic_compare_and_swap(ptr, cmp, exc) __sync_val_compare_and_swap(ptr, cmp, exc)
#endif

#endif //_ATOMIC_H_INCLUDED
