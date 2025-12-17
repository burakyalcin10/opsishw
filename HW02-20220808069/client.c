#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "task.h"

void print_menu() {
    printf("\n=== Task Processing Client ===\n");
    printf("1. Submit Task (Type 1: Reverse String)\n");
    printf("2. Submit Task (Type 2: Sum Integers)\n");
    printf("3. Submit Task (Type 3: Fibonacci)\n");
    printf("4. Get Result\n");
    printf("5. Exit\n");
    printf("Choose an option: ");
}

int main(int argc, char *argv[]) {
    CLIENT *clnt;
    char *host;
    
    if (argc < 2) {
        printf("usage: %s server_host\n", argv[0]);
        exit(1);
    }
    
    host = argv[1];
    
    /* Create RPC client */
    clnt = clnt_create(host, TASKPROG, TASKVERS, "tcp");
    if (clnt == NULL) {
        clnt_pcreateerror(host);
        exit(1);
    }
    
    printf("Connected to server at %s\n", host);
    
    int choice;
    while (1) {
        print_menu();
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input!\n");
            while (getchar() != '\n');
            continue;
        }
        getchar(); /* consume newline */
        
        if (choice == 5) {
            printf("Goodbye!\n");
            break;
        }
        
        if (choice >= 1 && choice <= 3) {
            /* Submit a task */
            task t;
            char payload[257];
            
            t.id = 0;  /* Will be set by server */
            t.type = choice;
            
            if (choice == 1) {
                printf("Enter string to reverse: ");
            } else if (choice == 2) {
                printf("Enter space-separated integers: ");
            } else {
                printf("Enter n for Fibonacci(n): ");
            }
            
            if (fgets(payload, sizeof(payload), stdin) == NULL) {
                printf("Error reading input!\n");
                continue;
            }
            
            /* Remove trailing newline */
            payload[strcspn(payload, "\n")] = 0;
            
            t.payload = payload;
            
            /* Call RPC */
            int *result_id = submit_task_1(&t, clnt);
            if (result_id == NULL) {
                clnt_perror(clnt, "submit_task_1 failed");
            } else {
                printf("Task submitted successfully! Task ID: %d\n", *result_id);
            }
            
        } else if (choice == 4) {
            /* Get a result */
            int task_id;
            printf("Enter task ID: ");
            if (scanf("%d", &task_id) != 1) {
                printf("Invalid input!\n");
                while (getchar() != '\n');
                continue;
            }
            getchar(); /* consume newline */
            
            /* Call RPC */
            result *res = get_result_1(&task_id, clnt);
            if (res == NULL) {
                clnt_perror(clnt, "get_result_1 failed");
            } else {
                printf("Task %d result: %s\n", res->id, res->output);
                /* Free XDR-allocated memory to prevent leak */
                clnt_freeres(clnt, (xdrproc_t)xdr_result, (caddr_t)res);
            }
            
        } else {
            printf("Invalid option!\n");
        }
    }
    
    clnt_destroy(clnt);
    return 0;
}
