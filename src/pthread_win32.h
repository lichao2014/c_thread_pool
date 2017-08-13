#ifndef _PTHREAD_WIN32_H_INCLUDED_
#define _PTHREAD_WIN32_H_INCLUDED_


#include <stddef.h>

typedef struct pthread_win32_t *pthread_t;

typedef struct {
    void  *stackaddr;
    size_t stacksize;
} pthread_attr_t;

int pthread_create(pthread_t *thread, pthread_attr_t *attr, void *(*start_routine)(void *), void *arg);

int pthread_join(pthread_t thread, void **retval);

int pthread_detach(pthread_t thread);

void pthread_exit(void *value_ptr);

pthread_t pthread_self(void);

int pthread_equal(pthread_t t1, pthread_t t2);


#endif //_PTHREAD_WIN32_H_INCLUDED_
