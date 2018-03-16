#include "thread_pool.h"

#include <stdlib.h>
#include "thread.h"
#include "mpmc_bound_queue.h"

struct thread_ctx_t {
    void(*fn)(void *);
    void *arg;
};

struct thread_work_t {
    struct mpmc_bound_queue_t *q;
    struct thread_work_t *steal;
    atomic_t stoped;
    pthread_t th;
    int id;
};

struct thread_pool_t {
    atomic_t round;
    int work_count;
    struct thread_work_t works[1];
};

static void
thread_ctx_copy(void *src, void *dst, int len) {
    struct thread_ctx_t *a = dst;
    struct thread_ctx_t *b =src;

    a->fn = b->fn;
    a->arg = b->arg;
}

static int *
thread_work_get_id() {
    static THREAD_LOCAL int tid = -1;
    return &tid;
}

static void * 
thread_work_fn(void *arg) {
    struct thread_work_t *w = arg;

    *thread_work_get_id() = w->id;

    struct thread_ctx_t ctx;
    while (!atomic_add_and_fetch(&w->stoped, 0)) {
        if (0 == mpmc_bound_queue_pop(w->q, &ctx, sizeof ctx) 
            || (w->steal && (0 == mpmc_bound_queue_pop(w->steal->q, &ctx, sizeof ctx)))) {
            ctx.fn(ctx.arg);
        }
        else {
            sched_yield();
        }
    }

    while (0 == mpmc_bound_queue_pop(w->q, &ctx, sizeof ctx)) {
        ctx.fn(ctx.arg);
    }

    return NULL;
}

static int 
thread_work_init(struct thread_work_t *w, int id, int queue_size) {
    w->q = mpmc_bound_queue_create(queue_size, &thread_ctx_copy);
    if (!w->q) {
        return -1;
    }

    w->id = id;
    w->stoped = 0;
    w->steal = NULL;

    return 0;
}

static int 
thread_work_start(struct thread_work_t *w, struct thread_work_t *steal) {
    w->stoped = 0;
    w->steal = steal;

    return pthread_create(&w->th, NULL, &thread_work_fn, w);
}

static int 
thread_work_stop(struct thread_work_t *w) {
    atomic_add_and_fetch(&w->stoped, 1);
    return pthread_join(w->th, NULL);
}

static void 
thread_work_term(struct thread_work_t *w) {
    mpmc_bound_queue_destroy(w->q);
}

int 
thread_work_submit(struct thread_work_t *w, void(*fn)(void *), void *arg) {
    if (atomic_add_and_fetch(&w->stoped, 0)) {
        return -1;
    }

    struct thread_ctx_t ctx;
    ctx.fn = fn;
    ctx.arg = arg;

    return mpmc_bound_queue_push(w->q, &ctx, sizeof ctx);
}

struct thread_pool_t *
thread_pool_create(int work_count, int queue_size) {
    struct thread_pool_t *tp = malloc(
        sizeof(struct thread_pool_t) 
        + sizeof(struct thread_work_t) * (work_count - 1));
    if (!tp) {
        return tp;
    }

    tp->round = 0;
    tp->work_count = work_count;

    int i;
    for (i = 0; i < work_count; ++i) {
        thread_work_init(tp->works + i, i, queue_size);
    }

    for (i = 0; i < work_count; ++i) {
        thread_work_start(tp->works + i, tp->works + ((i + 1) % work_count));
    }

    return tp;
}

int 
thread_pool_destroy(struct thread_pool_t *tp) {
    int i;
    for (i = 0; i < tp->work_count; ++i) {
        thread_work_stop(tp->works + i);
    }

    for (i = 0; i < tp->work_count; ++i) {
        thread_work_term(tp->works + i);
    }

    free(tp);

    return 0;
}

static struct thread_work_t *
thread_pool_get_work(struct thread_pool_t *tp) {
    int id = *thread_work_get_id();

    if (-1 == id) {
        id = atomic_add_and_fetch(&tp->round, 1) % tp->work_count;
    }

    return tp->works + id;
}

int 
thread_pool_submit(struct thread_pool_t *tp, void(*fn)(void *), void *arg) {
    return thread_work_submit(thread_pool_get_work(tp), fn, arg);
}
