#pragma once

typedef struct __response_t
{
	const char *status;
	const char *filename;
	const char *body;
} response_t;

void handle_connection(void *connected_sock);

void print_request(char *buffer);
