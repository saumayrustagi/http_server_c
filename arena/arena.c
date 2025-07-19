#include "arena.h"

memory_arena_t *arena_init(size_t size)
{
	memory_arena_t *arena;
	assert((arena = (memory_arena_t *)malloc(sizeof(memory_arena_t))) != NULL);
	assert((arena->mem_block = malloc(size)) != NULL);
	arena->next_free = arena->mem_block;
	arena->size = size;
	return arena;
}

void *arena_alloc(memory_arena_t *arena, size_t size);

void arena_reset(memory_arena_t *arena);

void arena_destroy(memory_arena_t *arena);
