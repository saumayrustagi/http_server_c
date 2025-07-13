#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

#include "threadpool/threadpool.h"
#include "server/server.h"
#include "server/connection.h"

void print_message(void *msg)
{
	printf("Task: %s (Thread ID: %ld)\n", (char *)msg, pthread_self());
}

int main()
{
	int listener = create_listener(strdup("127.0.0.1:8080"));
	print_listener_address(listener);
	while (1)
	{
		int connected_sock = accept(listener, NULL, NULL);
		handle_connection(connected_sock);
	}
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