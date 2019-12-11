#ifndef TPOOL_H
#define TPOOL_H

#include <pthread.h>

typedef struct _tpool_t tpool_t;

typedef void (*thread_func_t)(void *arg1, void *arg2);

tpool_t *ppg_tpool_create(uint32_t num);
void ppg_tpool_wait(tpool_t *tp);
bool ppg_tpool_add_work(tpool_t *tp, thread_func_t func, void *arg1, void *arg2);
void ppg_freeup_tpool(tpool_t *tp);

#endif
