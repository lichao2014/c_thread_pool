#ifndef _THREAD_POOL_H_INCLUDED
#define _THREAD_POOL_H_INCLUDED

struct thread_pool_t;

struct thread_pool_t *thread_pool_create(int work_count, int queue_size);
int thread_pool_destroy(struct thread_pool_t *tp);
int thread_pool_submit(struct thread_pool_t *tp, void(*fn)(void *), void *arg);

#endif //_THREAD_POOL_H_INCLUDED
