#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#include "threadpool.h"

void print_message(void *msg)
{
	printf("Task: %s (Thread ID: %ld)\n", (char *)msg, pthread_self());
}

void calculate_sum(int a, int b)
{
	printf("Calculating sum of %d + %d (Thread ID: %ld)\n", a, b, pthread_self());
	sleep(0.05);
	int sum = a + b;
	printf("Sum: %d (Thread ID: %ld) finished.\n", sum, pthread_self());
}

int main()
{
	threadpool_t *tp = threadpool_init(4, 16);
	threadpool_execute(tp, print_message, "Hello from task 1");
	threadpool_destroy(tp);
	return 0;
}