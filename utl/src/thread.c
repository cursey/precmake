#include <time.h>
#include "utl/macro.h"
#include "utl/thread.h"

thread_alloc ThreadLocals *thread_locals = 0;

void thread_locals_init_and_equip(ThreadLocals *locals)
{
    mem_zero_struct(locals);
    for (usize i = 0; i < array_count(locals->arenas); ++i)
    {
        locals->arenas[i] = arena_alloc();
    }
    locals->prng = prng_seed(time(0));
    thread_locals = locals;
}

void thread_locals_release(void)
{
    for (usize i = 0; i < array_count(thread_locals->arenas); ++i)
    {
        arena_release(thread_locals->arenas[i]);
    }
}

Arena *thread_locals_get_scratch(Arena **conflicts, usize count)
{
    Arena *result = 0;
    for (usize i = 0; i < array_count(thread_locals->arenas) && result == 0; i++)
    {
        bool has_conflict = false;
        for (usize j = 0; j < count; j++)
        {
            if (thread_locals->arenas[i] == conflicts[j])
            {
                has_conflict = true;
                break;
            }
        }
        if (!has_conflict)
        {
            result = thread_locals->arenas[i];
        }
    }
    return result;
}

Prng *thread_locals_prng(void)
{
    return &thread_locals->prng;
}