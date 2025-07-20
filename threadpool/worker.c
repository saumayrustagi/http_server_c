#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

#include "../queue/queue_task_t.h"
#include "worker.h"
#include "../arena/arena.h"

extern pthread_key_t thread_id;

extern size_t ARENA_SIZE;

void *worker(void *args)
{
	worker_args_t *worker_args = (worker_args_t *)args;
	queue_task_t *q = (queue_task_t *)worker_args->q;

	pthread_setspecific(thread_id, (void *)((intptr_t)worker_args->id));

	memory_arena_t *arena = arena_init(ARENA_SIZE);
	task_args_t *t_args;
	assert((t_args = malloc(sizeof(task_args_t))) != NULL);
	t_args->arena = arena;
	while (1)
	{
		arena_reset(arena);
		task_t task = dequeue(q);
		t_args->args = task.args;
		task.fn(t_args);
	}
	return NULL;
}
