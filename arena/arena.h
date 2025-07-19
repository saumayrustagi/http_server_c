#pragma once

#include <stddef.h>

typedef struct
{
	void *mem_block;
	void *next_free;
	size_t size;

} memory_arena_t;

memory_arena_t *arena_init(size_t size);

void *arena_alloc(memory_arena_t *arena, size_t size);

void arena_reset(memory_arena_t *arena);

void arena_destroy(memory_arena_t *arena);
