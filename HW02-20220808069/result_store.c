#include "result_store.h"
#include <stdio.h>
#include <string.h>

void result_store_init(result_store_t *store) {
    pthread_mutex_init(&store->mutex, NULL);
    
    /* Initialize all entries as invalid */
    for (int i = 0; i < MAX_RESULTS; i++) {
        store->entries[i].valid = 0;
        store->entries[i].task_id = -1;
    }
}

void result_store_put(result_store_t *store, int task_id, const char *output) {
    pthread_mutex_lock(&store->mutex);
    
    /* Find an empty slot or existing entry */
    int slot = -1;
    for (int i = 0; i < MAX_RESULTS; i++) {
        if (store->entries[i].task_id == task_id || !store->entries[i].valid) {
            slot = i;
            break;
        }
    }
    
    if (slot >= 0) {
        store->entries[slot].task_id = task_id;
        strncpy(store->entries[slot].output, output, 255);
        store->entries[slot].output[255] = '\0';
        store->entries[slot].valid = 1;
    } else {
        fprintf(stderr, "WARNING: Result store full! Task %d result lost.\n", task_id);
    }
    
    pthread_mutex_unlock(&store->mutex);
}

int result_store_get(result_store_t *store, int task_id, char *output) {
    pthread_mutex_lock(&store->mutex);
    
    int found = 0;
    for (int i = 0; i < MAX_RESULTS; i++) {
        if (store->entries[i].valid && store->entries[i].task_id == task_id) {
            strncpy(output, store->entries[i].output, 255);
            output[255] = '\0';
            found = 1;
            break;
        }
    }
    
    pthread_mutex_unlock(&store->mutex);
    return found;
}

void result_store_destroy(result_store_t *store) {
    pthread_mutex_destroy(&store->mutex);
}
