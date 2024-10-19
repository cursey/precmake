#pragma once

#include "utl/arena.h"
#include "utl/prng.h"

typedef struct ThreadLocals ThreadLocals;
struct ThreadLocals
{
    Arena *arenas[2];
    Prng prng;
};

void thread_locals_init_and_equip(ThreadLocals *locals);
void thread_locals_release(void);

Arena *thread_locals_get_scratch(Arena **conflicts, usize count);

Prng *thread_locals_prng(void);

#define scratch_begin(conflicts, count) temp_begin(thread_locals_get_scratch((conflicts), (count)))
#define scratch_end(scratch) temp_end(scratch)