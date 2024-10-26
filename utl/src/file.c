#include <stdio.h>
#include "utl/thread.h"
#include "utl/file.h"

Bytes file_read_all(Arena *arena, Str path)
{
    Temp scratch = scratch_begin(&arena, 1);
    Bytes result = {0};
    Str cpath = push_str_copy(scratch.arena, path);
    FILE *file = fopen((const char *)cpath.ptr, "rb");
    if (file != 0)
    {
        fseek(file, 0, SEEK_END);
        result.len = ftell(file);
        fseek(file, 0, SEEK_SET);
        result.ptr = push_array(arena, u8, result.len);
        fread(result.ptr, 1, result.len, file);
        fclose(file);
    }
    scratch_end(scratch);
    return result;
}

bool file_write_all(Str path, Str data)
{
    Temp scratch = scratch_begin(0, 0);
    Str cpath = push_str_copy(scratch.arena, path);
    FILE *file = fopen((const char *)cpath.ptr, "wb");
    if (file != 0)
    {
        fwrite(data.ptr, 1, data.len, file);
        fclose(file);
    }
    scratch_end(scratch);
    return file != 0;
}