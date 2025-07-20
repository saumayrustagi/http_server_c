#include <stdlib.h>
#include <assert.h>
#include <stdint.h>

#include "threadpool_t.h"
#include "worker.h"

pthread_key_t thread_id;

threadpool_t *threadpool_init(int num_threads, int queue_capacity)
{
	threadpool_t *tp;
	assert((tp = malloc(sizeof(threadpool_t))) != NULL);
	tp->num_threads = num_threads;
	assert((tp->workers = malloc(num_threads * sizeof(pthread_t))) != NULL);
	assert((tp->worker_args = malloc(num_threads * sizeof(worker_args_t))) != NULL);
	tp->queue = queue_init(queue_capacity);

	assert(pthread_key_create(&thread_id, NULL) == 0);

	for (int i = 0; i < num_threads; ++i)
	{
		tp->worker_args[i].id = i;
		tp->worker_args[i].q = tp->queue;
		pthread_create(&tp->workers[i], NULL, worker, &tp->worker_args[i]);
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
	assert(pthread_key_delete(thread_id) == 0);
	queue_destroy(tp->queue);
	free(tp->worker_args);
	free(tp->workers);
	free(tp);
}
