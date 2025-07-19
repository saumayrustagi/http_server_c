#include "arena.h"

memory_arena_t *arena_init(size_t capacity)
{
	memory_arena_t *arena;
	assert((arena = (memory_arena_t *)malloc(sizeof(memory_arena_t))) != NULL);
	assert((arena->mem_block = malloc(capacity)) != NULL);
	arena->next_free = arena->mem_block;
	arena->capacity = capacity;
	return arena;
}

void *arena_alloc(memory_arena_t *arena, size_t size)
{
	assert(arena != NULL && size > 0);
	void *curn_free = arena->next_free;
	arena->next_free = arena->mem_block + size;
	assert(arena->next_free - arena->mem_block < arena->capacity);
	return curn_free;
}

void arena_reset(memory_arena_t *arena)
{
	assert(arena != NULL);
	arena->next_free = arena->mem_block;
}

void arena_destroy(memory_arena_t *arena)
{
	free(arena->mem_block);
	free(arena);
}
