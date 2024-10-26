#pragma once

#include "utl/type.h"
#include "utl/arena.h"
#include "utl/str.h"

typedef struct Bytes Bytes;
struct Bytes
{
    u8 *ptr;
    usize len;
};

typedef struct BytesPattern BytesPattern;
struct BytesPattern
{
    usize len;
    i16 bytes[0];
};

typedef struct BytesNode BytesNode;
struct BytesNode
{
    BytesNode *next;
    Bytes bytes;
};

typedef struct BytesList BytesList;
struct BytesList
{
    BytesNode *head;
    BytesNode *tail;
    usize node_count;
    usize total_len;
};

Bytes make_bytes(u8 *ptr, usize len);
Str str_from_bytes(Bytes bytes);
Bytes bytes_from_str(Str str);
Bytes bytes_slice(Bytes bytes, usize start, usize len);

Bytes bytes_copy(Arena *arena, Bytes bytes);
Bytes bytes_copy_aligned(Arena *arena, Bytes bytes, usize alignment);

BytesPattern *make_bytes_pattern(Arena *arena, Str pattern);
BytesPattern *bytes_pattern_from_str(Arena *arena, Str str);

u8 *bytes_scan(Bytes bytes, Str pattern);
u8 *bytes_scan_pattern(Bytes bytes, BytesPattern *pattern);

BytesNode *bytes_list_push(Arena *arena, BytesList *list, Bytes bytes);
Bytes bytes_list_flatten(Arena *arena, BytesList *list);
