#define _POSIX_C_SOURCE 200809L

#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "threadpool/threadpool.h"
#include "web_server/server.h"

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
	int listener = create_listener(strdup("127.0.0.1:8080"));
	print_listener_address(listener);
	return 0;
}

// int main()
// {
// 	threadpool_t *tp = threadpool_init(4, 16);
// 	printf("Hi from main thread! (Thread ID: %ld)\n", pthread_self());
// 	threadpool_execute(tp, print_message, "Hello from task 1!");
// 	threadpool_destroy(tp);
// 	return 0;
// }