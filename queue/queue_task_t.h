#pragma once

#include <semaphore.h>

#include "../threadpool/task.h"

typedef struct __queue_task_t
{
	task_t *array;
	int size;
	int capacity;
	int head;
	int tail;

	sem_t mutex;
	sem_t empty;
	sem_t full;
} queue_task_t;

queue_task_t *queue_init(int capacity);

void queue_destroy(queue_task_t *q);

void enqueue(queue_task_t *q, task_t val);

task_t dequeue(queue_task_t *q);
