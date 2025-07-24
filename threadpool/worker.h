#pragma once

typedef struct __worker_args_t
{
	int id;
	queue_task_t *q;
} worker_args_t;

void *worker(void *args);
