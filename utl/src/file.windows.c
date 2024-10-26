#include <Windows.h>
#include "utl/thread.h"
#include "utl/utf.h"
#include "utl/file.h"

StrList file_walk(Arena *arena, Str directory)
{
    typedef struct Task Task;
    struct Task
    {
        Task *next;
        Str dir;
    };
    Task start_task = {.next = 0, .dir = directory};
    Task *head_task = &start_task;
    Task *tail_task = &start_task;
    StrList result = {0};
    Temp scratch = scratch_begin(&arena, 1);
    for (Task *task = head_task; task != 0; task = task->next)
    {
        Str pat = push_str_cat(scratch.arena, task->dir, str_lit("\\*"));
        Str16 pat16 = str16_from_8(scratch.arena, pat);
        WIN32_FIND_DATAW find_data = {0};
        HANDLE finder = FindFirstFileW(pat16.ptr, &find_data);
        while (finder != INVALID_HANDLE_VALUE && FindNextFileW(finder, &find_data))
        {
            Str name = str_from_16(scratch.arena, str16_from_cstr(find_data.cFileName));
            Str path = push_strf(scratch.arena, "%.*s\\%.*s", str_varg(task->dir), str_varg(name));
            if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                if (find_data.cFileName[0] != u'.')
                {
                    Task *next_task = push_array(scratch.arena, Task, 1);
                    next_task->dir = path;
                    queue_push(head_task, tail_task, next_task);
                }
            }
            else
            {
                str_list_push(arena, &result, push_str_copy(arena, path));
            }
        }
        FindClose(finder);
    }
    scratch_end(scratch);
    return result;
}

Str file_cwd(Arena *arena)
{
    Temp scratch = scratch_begin(&arena, 1);
    u32 len = GetCurrentDirectoryW(0, 0);
    Str16 result16;
    result16.ptr = push_array(scratch.arena, u16, len);
    result16.len = len - 1;
    GetCurrentDirectoryW(len, (LPWSTR)result16.ptr);
    Str result = str_from_16(arena, result16);
    scratch_end(scratch);
    return result;
}