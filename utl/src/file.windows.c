#include <Windows.h>
#include "utl/thread.h"
#include "utl/utf.h"
#include "utl/file.h"

Bytes file_read_all(Arena *arena, Str path)
{
    Bytes result = {0};
    Temp scratch = scratch_begin(&arena, 1);
    Str16 path16 = str16_from_8(scratch.arena, path);
    HANDLE file = CreateFileW(path16.ptr, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (file != INVALID_HANDLE_VALUE)
    {
        LARGE_INTEGER file_size;
        GetFileSizeEx(file, &file_size);
        void *buf = push_array(arena, u8, file_size.QuadPart);
        result.ptr = buf;
        result.len = file_size.QuadPart;

        // Read loop.
        u64 total_read = 0;
        while (total_read < file_size.QuadPart)
        {
            DWORD bytes_read;
            ReadFile(file, (u8 *)buf + total_read, file_size.QuadPart, &bytes_read, 0);
            total_read += bytes_read;
        }
        CloseHandle(file);
    }
    scratch_end(scratch);
    return result;
}

bool file_write_all(Str path, Str data)
{
    bool result = false;
    Temp scratch = scratch_begin(0, 0);
    Str16 path16 = str16_from_8(scratch.arena, path);
    HANDLE file = CreateFileW(path16.ptr, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
    if (file != INVALID_HANDLE_VALUE)
    {
        // Write loop.
        u64 total_written = 0;
        while (total_written < data.len)
        {
            DWORD bytes_written;
            WriteFile(file, data.ptr + total_written, data.len - total_written, &bytes_written, 0);
            total_written += bytes_written;
        }
        CloseHandle(file);
        result = true;
    }
    scratch_end(scratch);
    return result;
}

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