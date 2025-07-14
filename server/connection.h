#pragma once

typedef struct
{
	const char *status;
	const char *filename;
	const char *body;
} response_t;

void handle_connection(int connected_sock);