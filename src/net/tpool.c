#include <common.h>
#include <tpool.h>

/**
* README: Before reading look at referencs
* Also not doing any function error checking here
*/

/**
* Worker queue is a simple link list that stores
* (func): function call
* (server): ppg_server_t argument
* (arg): its argument
* (next): A pointer to the next worker
*/
typedef struct _tpool_work_t tpool_work_t;
struct _tpool_work_t {
  void *arg1;
  void *arg2;
  thread_func_t func;
  tpool_work_t *next;
};

/**
* Work queue implement as a linked list
* (wfirst, wlast): are used to push and pop objects
* (mux): is used for all locking
* (work_cond): signals threads that there is work to be processed
* (working_cond): signals when there are not threads processing
* (wcount): keeps count of how any threads are actively processing work
* (tcount): keeps count of how many threads are alive. Helps with preventing
*           running threads from being destroyed prematurely.
* (stop): actually stops thread, when used with tcount
*/
struct _tpool_t {
  tpool_work_t *wfirst;
  tpool_work_t *wlast;
  pthread_mutex_t mux;
  pthread_cond_t work_cond;
  pthread_cond_t working_cond;
  uint32_t wcount;
  uint32_t tcount;
  bool stop;
};

/* Create worker helper */
static tpool_work_t *tpool_work_create(thread_func_t func, void *arg1, void *arg2) {
  tpool_work_t *work = NULL;

  if (func == NULL) return NULL;

  work = calloc(1, sizeof(tpool_work_t));
  if (!work) {
    ppg_log_me(PPG_DANGER, "[x] calloc: %s", strerror(errno));
    return NULL;
  }

  work->func = func;
  work->arg1 = arg1;
  work->arg2 = arg2;
  work->next = NULL;

  return work;
}

/* Destory worker helper */
static void tpool_work_destroy(tpool_work_t *work) {
  FREE(work);
  return;
}

/**
* Get work/task from the queue to processed
* This function also helps maintain the linked list
* that is our queue
*/
static tpool_work_t *tpool_work_get(tpool_t *tp) {
  tpool_work_t *work = NULL;

  if (!tp) return NULL;

  work = tp->wfirst;
  if (!work) return NULL;

  if (!work->next) {
    tp->wfirst = NULL;
    tp->wlast = NULL;
  } else {
    tp->wfirst = work->next;
  }

  return work;
}

/**
* This is where work is handle
* line 96: keep thread running as long as it doesn't exit it can be used
* line 97: lock the mutex to ensure nothing else manipulates pool's members
* line 104: Check if pool requested that all threads stopped running and exit. Keeping the lock so that things
*           can be modified outside of loop (mux) will unlock before thread exits
* line 105: Check if work available and wait in a condition if there is none
* line 108: After thread is signaled now there's work, pull some from the queue
* line 109: inc wcount so that the pool knows there's thread is processing
* line 110: unlock mux so other threads an pull and process work
* line (115 - 119): if work process it and destory work obect
* line 130: Signal wait that a thread has exited.
*/
static void *tpool_worker(void *arg) {
  tpool_work_t *work = NULL;
  tpool_t *tp = (tpool_t *) arg;

  for (;;) {
    pthread_mutex_lock(&tp->mux);

    if (tp->stop) break;
    if (!tp->wfirst) pthread_cond_wait(&tp->work_cond, &tp->mux); /* will automatically lock and unlock mutex */

    work = tpool_work_get(tp);
    tp->wcount++;
    pthread_mutex_unlock(&tp->mux);

    if (work) {
      work->func(work->arg1, work->arg2);
      tpool_work_destroy(work);
    }

    pthread_mutex_lock(&tp->mux);
    tp->wcount--;
    if (!tp->stop && tp->wcount == 0 && !tp->wfirst)
      pthread_cond_signal(&tp->working_cond);

    pthread_mutex_unlock(&tp->mux);
  }

  tp->tcount--;
  pthread_cond_signal(&tp->working_cond);
  pthread_mutex_unlock(&tp->mux);
  return NULL;
}

/* line 146: if num == 0 default is to create 2 threads */
tpool_t *ppg_tpool_create(uint32_t num) {
  tpool_t *tp = NULL;
  pthread_t thread;

  num = (!num) ? 2 : num;

  tp = calloc(1, sizeof(tpool_t));
  if (!tp) {
    ppg_log_me(PPG_DANGER, "[x] calloc: %s", strerror(errno));
    return NULL;
  }

  tp->tcount = num;

  pthread_mutex_init(&tp->mux, NULL);
  pthread_cond_init(&tp->work_cond, NULL);
  pthread_cond_init(&tp->working_cond, NULL);

  tp->wfirst = NULL;
  tp->wlast = NULL;

  for (uint32_t i=0; i < num; i++) {
    pthread_create(&thread, NULL, tpool_worker, tp);
    pthread_detach(thread); /* Threads clean up on exit */
  }

  return tp;
}

/**
* Function adds work to the work queue.
* First by creating a work object
* Locking the mutex and adding the object to the liked list
*/
bool ppg_tpool_add_work(tpool_t *tp, thread_func_t func, void *arg1, void *arg2) {
  tpool_work_t *work = NULL;

  if (!tp) return false;

  work = tpool_work_create(func, arg1, arg2);
  if (!work) {
    ppg_log_me(PPG_DANGER, "[x] tpool_work_create failed");
    return false;
  }

  pthread_mutex_lock(&tp->mux);

  if (!tp->wfirst) {
    tp->wfirst = work;
    tp->wlast = tp->wfirst;
  } else {
    tp->wlast->next = work;
    tp->wlast = work;
  }

  pthread_cond_broadcast(&tp->work_cond);
  pthread_mutex_unlock(&tp->mux);

  return true;
}

/**
* Block function that will only return when there is no work
* The mutex is locked and code waits in a condition if there are
* threads processing, there is still work to do, or if threads are stopped,
* and not all hav exited.
*/
void ppg_tpool_wait(tpool_t *tp) {
  if (!tp) return;

  pthread_mutex_lock(&tp->mux);
  for (;;) {
    if ((!tp->stop && tp->wcount != 0) || (tp->stop && tp->tcount != 0)) {
      pthread_cond_wait(&tp->working_cond, &tp->mux);
    } else {
      break;
    }
  }
  pthread_mutex_unlock(&tp->mux);
}

/**
* Function does not interfere with anything processing because the threads have
* already pulled off the work they’re working on. Any threads trying to pull new work
* will wait until pending work is finished clearing out
*/
void ppg_freeup_tpool(tpool_t *tp) {
  tpool_work_t *work = NULL;
  tpool_work_t *work2 = NULL;

  if (!tp) return;

  /* we still have full access to the queue */
  pthread_mutex_lock(&tp->mux);
  work = tp->wfirst;

  while (work) {
    work2 = work->next;
    tpool_work_destroy(work); /* empty the queue */
    work = work2;
  }

  tp->stop = true;
  pthread_cond_broadcast(&tp->work_cond);
  pthread_mutex_unlock(&tp->mux);

  ppg_tpool_wait(tp); /* wait for threads to finish */

  pthread_mutex_destroy(&tp->mux);
  pthread_cond_destroy(&tp->work_cond);
  pthread_cond_destroy(&tp->working_cond);

  FREE(tp);
}
