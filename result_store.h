#ifndef RESULT_STORE_H
#define RESULT_STORE_H

#include <pthread.h>
#include "task.h"

#define MAX_RESULTS 1000

/* Storage entry for a task result */
typedef struct {
    int task_id;
    char output[257];
    int valid;  /* 1 if result is stored, 0 otherwise */
} result_entry_t;

/* Thread-safe result storage */
typedef struct {
    result_entry_t entries[MAX_RESULTS];
    pthread_mutex_t mutex;
} result_store_t;

/* Initialize the result store */
void result_store_init(result_store_t *store);

/* Store a result for a task ID (thread-safe) */
void result_store_put(result_store_t *store, int task_id, const char *output);

/* Get a result by task ID (thread-safe) 
 * Returns 1 if result found, 0 if not ready */
int result_store_get(result_store_t *store, int task_id, char *output);

/* Destroy the result store */
void result_store_destroy(result_store_t *store);

#endif /* RESULT_STORE_H */
