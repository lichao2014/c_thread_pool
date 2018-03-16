#ifndef _THREAD_H_INCLUDED
#define _THREAD_H_INCLUDED

#ifdef _WIN32
    #include <wtypes.h>
    #include "pthread_win32.h"

    #define THREAD_LOCAL       _declspec(thread)
    #define sched_yield()      Sleep(0)
#else
    #include <pthread.h>

    #define THREAD_LOCAL        __thread
#endif

#endif //_THREAD_H_INCLUDED
