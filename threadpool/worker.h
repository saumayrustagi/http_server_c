#pragma once

typedef struct
{
	int id;
	queue_task_t *q;
} worker_args_t;

void *worker(void *args);
