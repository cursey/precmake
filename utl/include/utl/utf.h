#pragma once

#include "utl/type.h"
#include "utl/arena.h"
#include "utl/str.h"

typedef struct UnicodeDecode UnicodeDecode;
struct UnicodeDecode
{
    u32 inc;
    u32 codepoint;
};

UnicodeDecode utf8_decode(u8 *str, usize max);
UnicodeDecode utf16_decode(u16 *str, usize max);
usize utf8_encode(u8 *str, u32 codepoint);
usize utf16_encode(u16 *str, u32 codepoint);

Str str_from_16(Arena *arena, Str16 in);
Str16 str16_from_8(Arena *arena, Str in);
Str str_from_32(Arena *arena, Str32 in);
Str32 str32_from_8(Arena *arena, Str in);
