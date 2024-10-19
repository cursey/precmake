#pragma once

#include "utl/type.h"
#include "utl/macro.h"
#include "utl/mem.h"

typedef struct ArenaParams ArenaParams;
struct ArenaParams
{
    usize reserve;
    usize commit;
};

typedef struct Arena Arena;
struct Arena
{
    usize reserve;
    usize commit;
    usize pos;
};

typedef struct Temp Temp;
struct Temp
{
    Arena *arena;
    usize pos;
};

Arena *arena_alloc_(ArenaParams *params);
#define arena_alloc(...) arena_alloc_(&(ArenaParams){.reserve = gigabyte(8), .commit = kilobyte(64), __VA_ARGS__})
void arena_release(Arena *arena);

void *arena_push(Arena *arena, usize size, usize align);
usize arena_pos(Arena *arena);
void arena_pop_to(Arena *arena, usize pos);

void arena_clear(Arena *arena);
void arena_pop(Arena *arena, usize amt);

#define push_array_no_zero_aligned(a, T, c, align) (T *)arena_push((a), sizeof(T) * (c), (align))
#define push_array_aligned(a, T, c, align) (T *)mem_zero(push_array_no_zero_aligned(a, T, c, align), sizeof(T) * (c))
#define push_array_no_zero(a, T, c) push_array_no_zero_aligned(a, T, c, align_of(T))
#define push_array(a, T, c) push_array_aligned(a, T, c, align_of(T))

Temp temp_begin(Arena *arena);
void temp_end(Temp temp);
