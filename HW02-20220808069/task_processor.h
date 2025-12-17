#ifndef TASK_PROCESSOR_H
#define TASK_PROCESSOR_H

#include "task.h"

/* Process a task based on its type and return the result */
void process_task(task *t, char *output);

/* Type 1: Reverse a string */
void reverse_string(const char *input, char *output);

/* Type 2: Sum space-separated integers */
void sum_integers(const char *input, char *output);

/* Type 3: Compute Fibonacci number */
void fibonacci(const char *input, char *output);

#endif /* TASK_PROCESSOR_H */
