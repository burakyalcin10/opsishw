#include "task_processor.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void process_task(task *t, char *output) {
    switch (t->type) {
        case 1:
            reverse_string(t->payload, output);
            break;
        case 2:
            sum_integers(t->payload, output);
            break;
        case 3:
            fibonacci(t->payload, output);
            break;
        default:
            strcpy(output, "ERROR: Invalid task type");
            break;
    }
}

void reverse_string(const char *input, char *output) {
    int len = strlen(input);
    for (int i = 0; i < len; i++) {
        output[i] = input[len - 1 - i];
    }
    output[len] = '\0';
}

void sum_integers(const char *input, char *output) {
    int sum = 0;
    char buffer[257];
    strncpy(buffer, input, 255);
    buffer[255] = '\0';
    
    char *token = strtok(buffer, " ");
    while (token != NULL) {
        sum += atoi(token);
        token = strtok(NULL, " ");
    }
    
    sprintf(output, "%d", sum);
}

void fibonacci(const char *input, char *output) {
    int n = atoi(input);
    
    if (n <= 0) {
        strcpy(output, "0");
        return;
    }
    
    if (n == 1) {
        strcpy(output, "1");
        return;
    }
    
    long long fib1 = 0, fib2 = 1, fib;
    
    for (int i = 2; i <= n; i++) {
        fib = fib1 + fib2;
        fib1 = fib2;
        fib2 = fib;
    }
    
    sprintf(output, "%lld", fib2);
}
