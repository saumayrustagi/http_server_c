#define _POSIX_C_SOURCE 200809L

#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <stdlib.h>
#include <pthread.h>

#include "connection.h"
#include "../threadpool/task_t.h"

typedef const char chararr[];

extern pthread_key_t thread_id;

static chararr GET_REQUEST = "GET / HTTP/1.1\r\n";
static chararr SLEEP_REQUEST = "GET /sleep HTTP/1.1\r\n";

static const int GET_REQUEST_LEN = sizeof(GET_REQUEST) - 1;
static const int SLEEP_REQUEST_LEN = sizeof(SLEEP_REQUEST) - 1;

static chararr OK_STATUS = "HTTP/1.1 200 OK \r\n\r\n";
static chararr NOTFOUND_STATUS = "HTTP/1.1 404 NOT FOUND \r\n\r\n";

void thread_print(char *str)
{
	fprintf(stderr, "[%ld][%ld] %s\n", pthread_self(), (long)pthread_getspecific(thread_id), str);
}

void print_request(char *buffer)
{
	char *foundptr;
	assert((foundptr = strstr(buffer, "\r\n")) != NULL);
	int req_len = (int)(foundptr - buffer);
	char line[req_len + 1];
	assert(snprintf(line, req_len + 1, "%s", buffer) > 0);
	thread_print(line);
}

void handle_connection(void *args)
{
	task_args_t *t_args = (task_args_t *)args;

	// Extract and free connected socket from task_args
	int connected_sock = *(int *)(t_args->args);
	free(t_args->args);
	t_args->args = NULL;

	// Extract arena from task_args
	memory_arena_t *arena = (memory_arena_t *)t_args->arena;

	// Start handling connection
	const size_t buf_size = 512;
	char *buffer = arena_alloc(arena, buf_size);
	ssize_t bytes_received = recv(connected_sock, buffer, buf_size - 1, 0);
	if (bytes_received <= 0)
	{
		close(connected_sock);
		return;
	}
	buffer[bytes_received] = '\0';

	print_request(buffer);

	response_t resp;
	resp.status = NULL;
	resp.filename = NULL;
	resp.body = NULL;

	if (strncmp(buffer, GET_REQUEST, GET_REQUEST_LEN) == 0)
	{
		resp.status = OK_STATUS;
		resp.filename = "hello.html";
	}
	else if (strncmp(buffer, SLEEP_REQUEST, SLEEP_REQUEST_LEN) == 0)
	{
		sleep(5);
		resp.status = OK_STATUS;
		resp.filename = "hello.html";
	}
	else
	{
		resp.status = NOTFOUND_STATUS;
	}

	if (resp.filename)
	{
		FILE *file;
		long filesize;
		char *body;
		size_t bytes_read;

		assert((file = fopen(resp.filename, "rb")) != NULL);
		assert(fseek(file, 0, SEEK_END) == 0);
		assert((filesize = ftell(file)) > 0);
		assert(fseek(file, 0, SEEK_SET) == 0);

		body = arena_alloc(arena, filesize + 1);
		assert((bytes_read = fread(body, sizeof(char), filesize, file)) == filesize);
		body[bytes_read] = '\0';

		assert(fclose(file) == 0);

		resp.body = body;
	}

	int unified_response_len = strlen(resp.status) + (resp.body ? strlen(resp.body) : 0) + 1;
	char *unified_response = arena_alloc(arena, unified_response_len);

	assert(snprintf(unified_response, unified_response_len, "%s%s", resp.status, (resp.body ? resp.body : "")) == unified_response_len - 1);

	send(connected_sock, unified_response, unified_response_len, 0);

	close(connected_sock);
	thread_print("====REQUEST SERVED====");
	fprintf(stderr, "%ld of %ld mem used\n", ((char *)arena->next_free - (char *)arena->mem_block), arena->capacity);
}
