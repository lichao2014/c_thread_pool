#ifndef _MPMC_BOUND_QUEUE_H_INCLUDED
#define _MPMC_BOUND_QUEUE_H_INCLUDED


#include "atomic.h"

#define CACHE_LINE_SIZE 64

typedef char cache_line_pad_t[CACHE_LINE_SIZE];

union mpmc_bound_queue_item_t {

    struct {
        atomic_t seq;
        char data[1];
    };
 
    cache_line_pad_t pad;
};

#define QUEUE_ITEM_DATA_MAX_SIZE  (CACHE_LINE_SIZE - sizeof(int64_t))

typedef void(*mpmc_bound_queue_item_copy_fn_t)(void *src, void *dst, int size);

struct mpmc_bound_queue_t {
    union {
        struct {
            mpmc_bound_queue_item_copy_fn_t copy_fn;
            int mask;
        };

        cache_line_pad_t pad0;
    };

    union {
        atomic_t enqueue_pos;
        cache_line_pad_t pad1;
    };

    union {
        atomic_t dequeue_pos;
        cache_line_pad_t pad2;
    };

    union mpmc_bound_queue_item_t items[1];
};

struct mpmc_bound_queue_t *mpmc_bound_queue_create(int size, mpmc_bound_queue_item_copy_fn_t fn);

void mpmc_bound_queue_destroy(struct mpmc_bound_queue_t *q);

int mpmc_bound_queue_push(struct mpmc_bound_queue_t *q, void *data, int size);

int mpmc_bound_queue_pop(struct mpmc_bound_queue_t *q, void *data, int size);


#endif //_MPMC_BOUND_QUEUE_H_INCLUDED
