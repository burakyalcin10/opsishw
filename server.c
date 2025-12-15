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
int server_initialized = 0;

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

/* Initialize server resources */
void server_init() {
    if (server_initialized) return;
    
    /* Initialize task queue and result store */
    task_queue_init(&task_queue);
    result_store_init(&result_store);
    
    /* Start worker threads */
    for (int i = 0; i < NUM_WORKERS; i++) {
        pthread_create(&workers[i], NULL, worker_thread, NULL);
    }
    
    printf("Server started with %d worker threads\n", NUM_WORKERS);
    server_initialized = 1;
}

/* RPC procedure: Submit a task */
int *submit_task_1_svc(task *argp, struct svc_req *rqstp) {
    static int result_id;
    
    /* Initialize server on first call */
    if (!server_initialized) {
        server_init();
    }
    
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
    
    return &result_id;
}

/* RPC procedure: Get result for a task */
result *get_result_1_svc(int *argp, struct svc_req *rqstp) {
    static result res;
    static char outbuf[256];
    
    /* Initialize server if not done (prevents crash) */
    if (!server_initialized) {
        server_init();
    }
    
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
    return &res;
}
