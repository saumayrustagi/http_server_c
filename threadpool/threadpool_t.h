#pragma once

#include "../queue/queue_task_t.h"
#include "worker.h"

typedef struct __threadpool_t
{
	int num_threads;
	pthread_t *workers;
	worker_args_t *worker_args;
	queue_task_t *queue;
} threadpool_t;

threadpool_t *threadpool_init(int num_threads, int queue_capacity);

void threadpool_execute(threadpool_t *tp, void (*function)(void *), void *args);

void threadpool_destroy(threadpool_t *tp);
