#pragma once

typedef struct
{
	void (*fn)(void *);
	void *args;
} task_t;
