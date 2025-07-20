#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <assert.h>
#include <stdlib.h>

#include "threadpool/threadpool_t.h"
#include "server/socket.h"
#include "server/connection.h"

const size_t ARENA_SIZE = 2048;

int main(void)
{
	int listener = create_listener(strdup("127.0.0.1:8080"));
	print_listener_address(listener);

	threadpool_t *pool = threadpool_init(4, 16);

	while (1)
	{
		int *connected_sock;
		assert((connected_sock = malloc(sizeof(int))) != NULL);
		assert((*connected_sock = accept(listener, NULL, NULL)) != -1);
		threadpool_execute(pool, handle_connection, connected_sock);
	}
	threadpool_destroy(pool);
	return 0;
}
