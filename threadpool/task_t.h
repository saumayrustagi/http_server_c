#pragma once

#include "../arena/arena.h"

typedef struct
{
	void (*fn)(void *);
	void *args;
} task_t;

typedef struct
{
	void *args;
	memory_arena_t *arena;
} task_args_t;
