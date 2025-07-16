#include "../queue/queue_task_t.h"

void *worker(void *args)
{
	queue_task_t *q = (queue_task_t *)args;
	while (1)
	{
		task_t task = dequeue(q);
		task.fn(task.args);
	}
	return NULL;
}
