#include "task_queue.h"
#include <stdlib.h>
#include <string.h>

void task_queue_init(task_queue_t *q) {
    q->head = NULL;
    q->tail = NULL;
    q->size = 0;
    pthread_mutex_init(&q->mutex, NULL);
    pthread_cond_init(&q->cond, NULL);
}

void task_queue_push(task_queue_t *q, task t) {
    queue_node_t *node = (queue_node_t *)malloc(sizeof(queue_node_t));
    node->t = t;
    
    /* Allocate and copy string payload */
    if (t.payload) {
        node->t.payload = strdup(t.payload);
    }
    
    node->next = NULL;
    
    pthread_mutex_lock(&q->mutex);
    
    if (q->tail == NULL) {
        /* Queue is empty */
        q->head = node;
        q->tail = node;
    } else {
        /* Append to tail */
        q->tail->next = node;
        q->tail = node;
    }
    
    q->size++;
    
    /* Signal waiting threads */
    pthread_cond_signal(&q->cond);
    pthread_mutex_unlock(&q->mutex);
}

task task_queue_pop(task_queue_t *q) {
    pthread_mutex_lock(&q->mutex);
    
    /* Wait while queue is empty */
    while (q->head == NULL) {
        pthread_cond_wait(&q->cond, &q->mutex);
    }
    
    /* Remove from head */
    queue_node_t *node = q->head;
    task t = node->t;
    
    q->head = q->head->next;
    if (q->head == NULL) {
        q->tail = NULL;
    }
    
    q->size--;
    
    pthread_mutex_unlock(&q->mutex);
    
    free(node);
    return t;
}

void task_queue_destroy(task_queue_t *q) {
    pthread_mutex_lock(&q->mutex);
    
    /* Free all remaining nodes */
    queue_node_t *current = q->head;
    while (current != NULL) {
        queue_node_t *next = current->next;
        if (current->t.payload) {
            free(current->t.payload);
        }
        free(current);
        current = next;
    }
    
    pthread_mutex_unlock(&q->mutex);
    pthread_mutex_destroy(&q->mutex);
    pthread_cond_destroy(&q->cond);
}
