#include "mpmc_bound_queue.h"

#include <assert.h>
#include <stdlib.h>

struct mpmc_bound_queue_t *
mpmc_bound_queue_create(int size, mpmc_bound_queue_item_copy_fn_t fn) {
    if (size & (size - 1)) {
        assert(0);
    }

    struct mpmc_bound_queue_t *q = malloc(
        sizeof(struct mpmc_bound_queue_t) 
        + sizeof(union mpmc_bound_queue_item_t) * (size - 1));
    if (!q) {
        return q;
    }

    q->copy_fn = fn;
    q->mask = size - 1;
    q->dequeue_pos = 0;
    q->enqueue_pos = 0;

    int i;
    for (i = 0; i < size; ++i) {
        q->items[i].seq = i;
    }

    return q;
}

void 
mpmc_bound_queue_destroy(struct mpmc_bound_queue_t *q) {
    free(q);
}

int 
mpmc_bound_queue_push(struct mpmc_bound_queue_t *q, void *data, int size) {
    union mpmc_bound_queue_item_t *item;

    atomic_t pos = atomic_add_and_fetch(&q->enqueue_pos, 0);
    while (1) {
        item = &q->items[pos & q->mask];
        atomic_t seq = atomic_add_and_fetch(&item->seq, 0);
        long long diff = (long long)seq - (long long)pos;
        if (0 == diff) {
            pos = atomic_compare_and_swap(&q->enqueue_pos, pos, pos + 1);
            if (pos == seq) {
                break;
            }
        } else if (diff < 0) {
            return -1;
        } else {
            pos = atomic_add_and_fetch(&q->enqueue_pos, 0);
        }
    }

    q->copy_fn(data, item->data, size);
    atomic_add_and_fetch(&item->seq, 1);

    return 0;
}

int 
mpmc_bound_queue_pop(struct mpmc_bound_queue_t *q, void *data, int size) {
    union mpmc_bound_queue_item_t *item;

    atomic_t pos = atomic_add_and_fetch(&q->dequeue_pos, 0);
    while (1) {
        item = &q->items[pos & q->mask];
        atomic_t seq = atomic_add_and_fetch(&item->seq, 0);
        long long diff = (long long)seq - (long long)(pos + 1);
        if (0 == diff) {
            pos = atomic_compare_and_swap(&q->dequeue_pos, pos, pos + 1);
            if (pos + 1 == seq) {
                break;
            }
        } else if (diff < 0) {
            return -1;
        } else {
            pos = atomic_add_and_fetch(&q->dequeue_pos, 0);
        }
    }

    if (data) {
        q->copy_fn(item->data, data, size);
    }

    atomic_add_and_fetch(&item->seq, q->mask);

    return 0;
}
