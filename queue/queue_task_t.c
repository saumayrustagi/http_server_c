#include <stdlib.h>
#include <assert.h>

#include "queue_task_t.h"

queue_task_t *queue_init(int capacity)
{
	queue_task_t *q;

	assert((q = (queue_task_t *)malloc(sizeof(queue_task_t))) != NULL);
	assert((q->array = malloc(sizeof(task_t) * capacity)) != NULL);

	q->size = 0;
	q->capacity = capacity;
	q->head = 0;
	q->tail = 0;

	assert(sem_init(&q->mutex, 0, 1) == 0);
	assert(sem_init(&q->empty, 0, capacity) == 0);
	assert(sem_init(&q->full, 0, 0) == 0);

	return q;
}

void queue_destroy(queue_task_t *q)
{
	free(q->array);
	assert(sem_destroy(&q->mutex) == 0);
	assert(sem_destroy(&q->empty) == 0);
	assert(sem_destroy(&q->full) == 0);
	free(q);
}

void enqueue(queue_task_t *q, task_t val)
{
	sem_wait(&q->empty);

	sem_wait(&q->mutex);

	q->array[q->tail] = val;
	++(q->size);
	q->tail = (q->tail < q->capacity - 1) ? q->tail + 1 : 0;

	sem_post(&q->mutex);

	sem_post(&q->full);
}

task_t dequeue(queue_task_t *q)
{
	sem_wait(&q->full);

	sem_wait(&q->mutex);

	task_t tmp = q->array[q->head];
	--(q->size);
	q->head = (q->head < q->capacity - 1) ? q->head + 1 : 0;

	sem_post(&q->mutex);

	sem_post(&q->empty);

	return tmp;
}
