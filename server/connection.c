#define _POSIX_C_SOURCE 200809L

#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <stdlib.h>

#include "connection.h"

typedef const char chararr[];

static chararr GET_REQUEST = "GET / HTTP/1.1\r\n";
static chararr SLEEP_REQUEST = "GET /sleep HTTP/1.1\r\n";

static const int GET_REQUEST_LEN = sizeof(GET_REQUEST) - 1;
static const int SLEEP_REQUEST_LEN = sizeof(SLEEP_REQUEST) - 1;

static chararr OK_STATUS = "HTTP/1.1 200 OK \r\n\r\n";
static chararr NOTFOUND_STATUS = "HTTP/1.1 404 NOT FOUND \r\n\r\n";

void print_request(char *buffer)
{
	char *foundptr;
	assert((foundptr = strstr(buffer, "\r\n")) != NULL);
	int req_len = (int)(foundptr - buffer);
	char line[req_len + 1];
	assert(snprintf(line, req_len + 1, "%s", buffer) > 0);
	fprintf(stderr, "%s\n", line);
}

void handle_connection(void *args)
{
	// Extract connected socket
	int connected_sock = *(int *)args;
	// Free Argument
	free(args);
	args = NULL;

	// Start handling connection
	char buffer[512];
	ssize_t bytes_received = recv(connected_sock, buffer, sizeof(buffer) - 1, 0);
	assert(bytes_received > 0);
	buffer[bytes_received] = '\0';

	print_request(buffer);

	response_t resp;
	resp.status = NULL;
	resp.filename = NULL;
	assert((resp.body = strdup("")) != NULL);

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

		assert((body = malloc(filesize + 1)) != NULL);
		assert((bytes_read = fread(body, sizeof(char), filesize, file)) == filesize);
		body[bytes_read] = '\0';

		assert(fclose(file) == 0);

		free((char *)resp.body);
		resp.body = body;
	}

	int unified_response_len = strlen(resp.status) + strlen(resp.body) + 1;
	char *unified_response = malloc(unified_response_len);

	assert(snprintf(unified_response, unified_response_len, "%s%s", resp.status, resp.body) == unified_response_len - 1);

	send(connected_sock, unified_response, unified_response_len, 0);

	free(unified_response);
	free((char *)resp.body);

	close(connected_sock);
	fprintf(stderr, "%s\n", "====REQUEST SERVED====");
}
