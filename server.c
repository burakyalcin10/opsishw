#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include "task.h"
#include "task_queue.h"
#include "result_store.h"
#include "task_processor.h"

#define NUM_WORKERS 3

/* Global shared data structures */
task_queue_t task_queue;
result_store_t result_store;
int next_task_id = 1;
pthread_mutex_t id_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_t workers[NUM_WORKERS];

/* Thread-safe initialization using pthread_once */
pthread_once_t init_once = PTHREAD_ONCE_INIT;

/* Worker thread function */
void *worker_thread(void *arg) {
    while (1) {
        /* Pop a task from the queue (blocking) */
        task t = task_queue_pop(&task_queue);
        
        /* Process the task */
        char output[256];
        process_task(&t, output);
        
        /* Store the result */
        result_store_put(&result_store, t.id, output);
        
        /* Free the payload string */
        if (t.payload) {
            free(t.payload);
        }
        
        printf("Worker %ld processed task %d (type %d): %s\n", 
               (long)pthread_self(), t.id, t.type, output);
    }
    return NULL;
}

/* Initialize server resources (called once via pthread_once) */
void server_init_impl() {
    /* Initialize task queue and result store */
    task_queue_init(&task_queue);
    result_store_init(&result_store);
    
    /* Start worker threads */
    for (int i = 0; i < NUM_WORKERS; i++) {
        pthread_create(&workers[i], NULL, worker_thread, NULL);
    }
    
    printf("Server started with %d worker threads\n", NUM_WORKERS);
}

/* Thread-safe server initialization wrapper */
void server_init() {
    pthread_once(&init_once, server_init_impl);
}

/* Mutex for RPC handler static variables (thread-safety) */
pthread_mutex_t rpc_handler_mutex = PTHREAD_MUTEX_INITIALIZER;

/* RPC procedure: Submit a task */
int *submit_task_1_svc(task *argp, struct svc_req *rqstp) {
    static int result_id;
    
    /* Ensure server is initialized (thread-safe) */
    server_init();
    
    /* Lock for static variable protection */
    pthread_mutex_lock(&rpc_handler_mutex);
    
    /* Generate unique task ID */
    pthread_mutex_lock(&id_mutex);
    result_id = next_task_id++;
    pthread_mutex_unlock(&id_mutex);
    
    /* Set task ID */
    argp->id = result_id;
    
    /* Push task to queue */
    task_queue_push(&task_queue, *argp);
    
    printf("Task %d submitted (type %d, payload: %s)\n", 
           result_id, argp->type, argp->payload);
    
    pthread_mutex_unlock(&rpc_handler_mutex);
    return &result_id;
}

/* RPC procedure: Get result for a task */
result *get_result_1_svc(int *argp, struct svc_req *rqstp) {
    static result res;
    static char outbuf[256];
    
    /* Ensure server is initialized (thread-safe) */
    server_init();
    
    /* Lock for static variable protection */
    pthread_mutex_lock(&rpc_handler_mutex);
    
    res.id = *argp;
    
    /* Try to get result from store */
    if (result_store_get(&result_store, *argp, outbuf)) {
        /* Result is ready - outbuf already filled */
    } else {
        /* Result not ready yet */
        strncpy(outbuf, "PENDING", sizeof(outbuf));
        outbuf[255] = '\0';
    }
    
    res.output = outbuf;
    
    pthread_mutex_unlock(&rpc_handler_mutex);
    return &res;
}
