#pragma once

#include "utl/arena.h"
#include "utl/str.h"
#include "utl/bytes.h"

Bytes file_read_all(Arena *arena, Str path);
bool file_write_all(Str path, Str data);
StrList file_walk(Arena *arena, Str directory);
Str file_cwd(Arena *arena);
