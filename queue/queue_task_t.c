#include <stdlib.h>
#include <assert.h>

#include "queue_task_t.h"

queue_task_t *queue_init(int capacity)
{
	queue_task_t *q;
	assert((q = (queue_task_t *)malloc(sizeof(queue_task_t))) != NULL);
	q->array = malloc(sizeof(task_t) * capacity);
	q->size = 0;
	q->capacity = capacity;
	q->head = 0;
	q->tail = 0;

	pthread_mutex_init(&q->lock, NULL);
	pthread_cond_init(&q->not_empty, NULL);
	pthread_cond_init(&q->not_full, NULL);
	return q;
}

void queue_destroy(queue_task_t *q)
{
	free(q->array);
	pthread_mutex_destroy(&q->lock);
	pthread_cond_destroy(&q->not_empty);
	pthread_cond_destroy(&q->not_full);
	free(q);
}

void enqueue(queue_task_t *q, task_t val)
{
	pthread_mutex_lock(&q->lock);
	while (q->size == q->capacity)
	{
		pthread_cond_wait(&q->not_full, &q->lock);
	}
	q->array[q->tail] = val;
	++(q->size);
	q->tail = (q->tail < q->capacity - 1) ? q->tail + 1 : 0;
	pthread_cond_signal(&q->not_empty);
	pthread_mutex_unlock(&q->lock);
}

task_t dequeue(queue_task_t *q)
{
	pthread_mutex_lock(&q->lock);
	while (q->size == 0)
	{
		pthread_cond_wait(&q->not_empty, &q->lock);
	}
	task_t tmp = q->array[q->head];
	--(q->size);
	q->head = (q->head < q->capacity - 1) ? q->head + 1 : 0;
	pthread_cond_signal(&q->not_full);
	pthread_mutex_unlock(&q->lock);
	return tmp;
}

