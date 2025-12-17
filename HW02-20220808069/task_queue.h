#ifndef TASK_QUEUE_H
#define TASK_QUEUE_H

#include <pthread.h>
#include "task.h"

/* Node structure for linked list queue */
typedef struct queue_node {
    task t;
    struct queue_node *next;
} queue_node_t;

/* Thread-safe FIFO task queue */
typedef struct {
    queue_node_t *head;
    queue_node_t *tail;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int size;
} task_queue_t;

/* Initialize the task queue */
void task_queue_init(task_queue_t *q);

/* Push a task to the queue (thread-safe) */
void task_queue_push(task_queue_t *q, task t);

/* Pop a task from the queue (blocking, thread-safe) */
task task_queue_pop(task_queue_t *q);

/* Destroy the task queue */
void task_queue_destroy(task_queue_t *q);

#endif /* TASK_QUEUE_H */
