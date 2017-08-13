#include "pthread_win32.h"
#include <wtypes.h>
#include <process.h>


struct pthread_win32_t {
    HANDLE nh;
    void *(*routine)(void *);
    void *arg;
    void *ret;
    int detach;
};


__declspec(thread) struct pthread_win32_t *win32_thread_self;


static unsigned WINAPI
win32_thread_routine(void *arg)
{
    struct pthread_win32_t *p = (struct pthread_win32_t *)arg;
    win32_thread_self = p;

    p->ret = p->routine(p->arg);

    if (p->detach) {
        CloseHandle(p->nh);
        free(p);
    }

    return 0;
}


int 
pthread_create(pthread_t *thread, pthread_attr_t *attr, void *(*start_routine)(void *), void *arg)
{
    int err;
    struct pthread_win32_t *p;

    p = (struct pthread_win32_t *)malloc(sizeof(struct pthread_win32_t));
    if (!p) {
        return ENOMEM;
    }

    p->routine = start_routine;
    p->arg = arg;
    p->ret = NULL;
    p->detach = 0;
    p->nh = (HANDLE)_beginthreadex(NULL,
        0,
        &win32_thread_routine,
        p,
        CREATE_SUSPENDED,
        NULL);

    if (!p->nh) {
        err = errno;
        free(p);
        return err;
    }

    *thread = p;
    ResumeThread(p->nh);

    return 0;
}


int 
pthread_join(pthread_t thread, void **retval)
{
    DWORD dw;

    dw = WaitForSingleObject(thread->nh, INFINITE);
    if (WAIT_OBJECT_0 != dw) {
        return -1;
    }

    if (thread->detach) {
        return EINVAL;
    }

    if (retval) {
        *retval = thread->ret;
    }

    CloseHandle(thread->nh);
    free(thread);

    return 0;
}


int 
pthread_detach(pthread_t thread)
{
    thread->detach = 1;
    return 0;
}


void 
pthread_exit(void *value_ptr)
{
    win32_thread_self->ret = value_ptr;
    _endthreadex(0);
}


pthread_t 
pthread_self(void)
{
    if (!win32_thread_self) {
        static struct pthread_win32_t main_thread;
        main_thread.nh = GetCurrentThread();
        main_thread.detach = 1;

        win32_thread_self = &main_thread;
    }

    return win32_thread_self;
}


int 
pthread_equal(pthread_t t1, pthread_t t2)
{
    return t1->nh != t2->nh;
}
