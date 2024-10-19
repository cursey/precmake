#include "utl/macro.h"
#include "utl/arena.h"

Arena *arena_alloc_(ArenaParams *params)
{
    usize reserve_size = align_up(params->reserve, 0x1000);
    usize commit_size = align_up(params->commit, 0x1000);
    void *buf = mem_reserve(reserve_size);
    mem_commit(buf, commit_size);
    Arena *arena = buf;
    arena->reserve = reserve_size;
    arena->commit = commit_size;
    arena->pos = sizeof(Arena);
    return arena;
}

void arena_release(Arena *arena)
{
    mem_release(arena, arena->reserve);
}

void *arena_push(Arena *arena, usize size, usize align)
{
    usize pos_pre = align_up(arena->pos, align);
    usize pos_pst = pos_pre + size;

    // Commit additional pages if needed.
    if (arena->commit < pos_pst)
    {
        usize commit_pst_aligned = align_up(pos_pst, kilobyte(64));
        usize commit_pst_clamped = n_min(commit_pst_aligned, arena->reserve);
        usize commit_size = commit_pst_clamped - arena->commit;
        mem_commit((u8 *)arena + arena->commit, commit_size);
        arena->commit = commit_pst_clamped;
    }
    void *result = 0;
    if (arena->commit >= pos_pst)
    {
        result = (char *)arena + pos_pre;
        arena->pos = pos_pst;
    }
    return result;
}

usize arena_pos(Arena *arena)
{
    return arena->pos;
}

void arena_pop_to(Arena *arena, usize pos)
{
    pos = n_max(sizeof(Arena), pos);
    arena->pos = pos;
}

void arena_clear(Arena *arena)
{
    arena_pop_to(arena, 0);
}

void arena_pop(Arena *arena, usize amt)
{
    usize pos_old = arena_pos(arena);
    usize pos_new = pos_old;
    if (pos_old > amt)
    {
        pos_new = pos_old - amt;
    }
    arena_pop_to(arena, pos_new);
}

Temp temp_begin(Arena *arena)
{
    Temp temp = {.arena = arena, .pos = arena_pos(arena)};
    return temp;
}

void temp_end(Temp temp)
{
    arena_pop_to(temp.arena, temp.pos);
}
