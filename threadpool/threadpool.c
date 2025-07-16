#include <stdlib.h>
#include <assert.h>

#include "threadpool.h"
#include "worker.h"

threadpool_t *threadpool_init(int num_threads, int queue_capacity)
{
	threadpool_t *tp;
	assert((tp = malloc(sizeof(threadpool_t))) != NULL);
	tp->num_threads = num_threads;
	assert((tp->workers = malloc(num_threads * sizeof(pthread_t))) != NULL);
	tp->queue = queue_init(queue_capacity);

	for (int i = 0; i < num_threads; ++i)
	{
		pthread_create(&tp->workers[i], NULL, worker, tp->queue);
	}
	return tp;
}

void threadpool_execute(threadpool_t *tp, void (*function)(void *), void *args)
{
	task_t task;
	task.fn = function;
	task.args = args;
	enqueue(tp->queue, task);
}

void threadpool_destroy(threadpool_t *tp)
{
	for (int i = 0; i < tp->num_threads; ++i)
	{
		pthread_join(tp->workers[i], NULL);
	}
	queue_destroy(tp->queue);
	free(tp->workers);
	free(tp);
}
