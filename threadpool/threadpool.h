#pragma once

#include "../queue/queue_task_t.h"

typedef struct
{
	int num_threads;
	pthread_t *workers;
	queue_task_t *queue;
} threadpool_t;

threadpool_t *threadpool_init(int num_threads, int queue_capacity);

void threadpool_execute(threadpool_t *tp, void (*function)(void *), void *args);

void threadpool_destroy(threadpool_t *tp);
