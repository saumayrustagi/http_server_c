#include <pthread.h>
#include <stdint.h>

#include "../queue/queue_task_t.h"
#include "worker.h"

extern pthread_key_t thread_id;

void *worker(void *args)
{
	worker_args_t *worker_args = (worker_args_t *)args;
	queue_task_t *q = (queue_task_t *)worker_args->q;

	pthread_setspecific(thread_id, (void *)((intptr_t)worker_args->id));

	while (1)
	{
		task_t task = dequeue(q);
		task.fn(task.args);
	}
	return NULL;
}
