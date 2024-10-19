#include "utl/const.h"
#include "utl/utf.h"

u8 utf8_class[32] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 3, 3, 4, 5,
};

UnicodeDecode utf8_decode(u8 *str, usize max)
{
    UnicodeDecode result = {.inc = 1, .codepoint = max_u32};
    u8 byte = str[0];
    u8 byte_class = utf8_class[byte >> 3];
    switch (byte_class)
    {
    case 1:
        result.codepoint = byte;
        break;
    case 2:
        if (2 < max)
        {
            u8 cont_byte = str[1];
            if (utf8_class[cont_byte >> 3] == 0)
            {
                result.codepoint = (byte & bitmask5) << 6;
                result.codepoint |= (cont_byte & bitmask6);
                result.inc = 2;
            }
        }
        break;
    case 3:
        if (2 < max)
        {
            u8 cont_byte[2] = {str[1], str[2]};
            if (utf8_class[cont_byte[0] >> 3] == 0 && utf8_class[cont_byte[1] >> 3] == 0)
            {
                result.codepoint = (byte & bitmask4) << 12;
                result.codepoint |= ((cont_byte[0] & bitmask6) << 6);
                result.codepoint |= (cont_byte[1] & bitmask6);
                result.inc = 3;
            }
        }
        break;
    case 4:
        if (3 < max)
        {
            u8 cont_byte[3] = {str[1], str[2], str[3]};
            if (utf8_class[cont_byte[0] >> 3] == 0 && utf8_class[cont_byte[1] >> 3] == 0 &&
                utf8_class[cont_byte[2] >> 3] == 0)
            {
                result.codepoint = (byte & bitmask3) << 18;
                result.codepoint |= ((cont_byte[0] & bitmask6) << 12);
                result.codepoint |= ((cont_byte[1] & bitmask6) << 6);
                result.codepoint |= (cont_byte[2] & bitmask6);
                result.inc = 4;
            }
        }
    }
    return result;
}

UnicodeDecode utf16_decode(u16 *str, usize max)
{
    UnicodeDecode result = {1, max_u32};
    result.codepoint = str[0];
    result.inc = 1;
    if (max > 1 && 0xD800 <= str[0] && str[0] < 0xDC00 && 0xDC00 <= str[1] && str[1] < 0xE000)
    {
        result.codepoint = ((str[0] - 0xD800) << 10) | ((str[1] - 0xDC00) + 0x10000);
        result.inc = 2;
    }
    return result;
}

usize utf8_encode(u8 *str, u32 codepoint)
{
    u32 inc = 0;
    if (codepoint <= 0x7F)
    {
        str[0] = (u8)codepoint;
        inc = 1;
    }
    else if (codepoint <= 0x7FF)
    {
        str[0] = (bitmask2 << 6) | ((codepoint >> 6) & bitmask5);
        str[1] = bit8 | (codepoint & bitmask6);
        inc = 2;
    }
    else if (codepoint <= 0xFFFF)
    {
        str[0] = (bitmask3 << 5) | ((codepoint >> 12) & bitmask4);
        str[1] = bit8 | ((codepoint >> 6) & bitmask6);
        str[2] = bit8 | (codepoint & bitmask6);
        inc = 3;
    }
    else if (codepoint <= 0x10FFFF)
    {
        str[0] = (bitmask4 << 4) | ((codepoint >> 18) & bitmask3);
        str[1] = bit8 | ((codepoint >> 12) & bitmask6);
        str[2] = bit8 | ((codepoint >> 6) & bitmask6);
        str[3] = bit8 | (codepoint & bitmask6);
        inc = 4;
    }
    else
    {
        str[0] = '?';
        inc = 1;
    }
    return inc;
}

usize utf16_encode(u16 *str, u32 codepoint)
{
    u32 inc = 1;
    if (codepoint == max_u32)
    {
        str[0] = (u16)'?';
    }
    else if (codepoint < 0x10000)
    {
        str[0] = (u16)codepoint;
    }
    else
    {
        u32 v = codepoint - 0x10000;
        str[0] = (u16)(0xD800 + (v >> 10));
        str[1] = (u16)(0xDC00 + (v & bitmask10));
        inc = 2;
    }
    return (inc);
}

Str str_from_16(Arena *arena, Str16 in)
{
    u64 cap = in.len * 3;
    u8 *str = push_array_no_zero(arena, u8, cap + 1);
    u16 *ptr = in.ptr;
    u16 *opl = ptr + in.len;
    u64 size = 0;
    UnicodeDecode consume;
    for (; ptr < opl; ptr += consume.inc)
    {
        consume = utf16_decode(ptr, opl - ptr);
        size += utf8_encode(str + size, consume.codepoint);
    }
    str[size] = 0;
    arena_pop(arena, (cap - size));
    return make_str(str, size);
}

Str16 str16_from_8(Arena *arena, Str in)
{
    u64 cap = in.len * 2;
    u16 *str = push_array_no_zero(arena, u16, cap + 1);
    u8 *ptr = in.ptr;
    u8 *opl = ptr + in.len;
    u64 size = 0;
    UnicodeDecode consume;
    for (; ptr < opl; ptr += consume.inc)
    {
        consume = utf8_decode(ptr, opl - ptr);
        size += utf16_encode(str + size, consume.codepoint);
    }
    str[size] = 0;
    arena_pop(arena, (cap - size) * 2);
    return make_str16(str, size);
}

Str str_from_32(Arena *arena, Str32 in)
{
    u64 cap = in.len * 4;
    u8 *str = push_array_no_zero(arena, u8, cap + 1);
    u32 *ptr = in.ptr;
    u32 *opl = ptr + in.len;
    u64 size = 0;
    for (; ptr < opl; ptr += 1)
    {
        size += utf8_encode(str + size, *ptr);
    }
    str[size] = 0;
    arena_pop(arena, (cap - size));
    return make_str(str, size);
}

Str32 str32_from_8(Arena *arena, Str in)
{
    u64 cap = in.len;
    u32 *str = push_array_no_zero(arena, u32, cap + 1);
    u8 *ptr = in.ptr;
    u8 *opl = ptr + in.len;
    u64 size = 0;
    UnicodeDecode consume;
    for (; ptr < opl; ptr += consume.inc)
    {
        consume = utf8_decode(ptr, opl - ptr);
        str[size] = consume.codepoint;
        size += 1;
    }
    str[size] = 0;
    arena_pop(arena, (cap - size) * 4);
    return make_str32(str, size);
}
