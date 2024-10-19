#include "utl/thread.h"
#include "utl/bytes.h"

Bytes make_bytes(u8 *ptr, usize len)
{
    return (Bytes){.ptr = ptr, .len = len};
}

Str str_from_bytes(Bytes bytes)
{
    return make_str(bytes.ptr, bytes.len);
}

Bytes bytes_from_str(Str str)
{
    return make_bytes(str.ptr, str.len);
}

Bytes bytes_slice(Bytes bytes, usize start, usize len)
{
    if (start >= bytes.len)
    {
        return make_bytes(0, 0);
    }
    if (len > bytes.len - start)
    {
        len = bytes.len - start;
    }
    return make_bytes(bytes.ptr + start, len);
}

Bytes bytes_copy(Arena *arena, Bytes bytes)
{
    Bytes result = make_bytes(push_array(arena, u8, bytes.len), bytes.len);
    mem_copy(result.ptr, bytes.ptr, bytes.len);
    return result;
}

Bytes bytes_copy_aligned(Arena *arena, Bytes bytes, usize alignment)
{
    Bytes result = make_bytes(push_array_aligned(arena, u8, bytes.len, alignment), bytes.len);
    mem_copy(result.ptr, bytes.ptr, bytes.len);
    return result;
}

static u8 hex_to_u8(u8 ch)
{
    if (ch >= '0' && ch <= '9')
    {
        return ch - '0';
    }
    if (ch >= 'A' && ch <= 'F')
    {
        return ch - 'A' + 10;
    }
    if (ch >= 'a' && ch <= 'f')
    {
        return ch - 'a' + 10;
    }
    return 0;
}

// Builds a byte pattern from a Str in the form of "AB CD ? EF ? ? 12 34" where '?' is a wildcard byte.
BytesPattern *make_bytes_pattern(Arena *arena, Str pattern)
{
    BytesPattern *result = push_array(arena, BytesPattern, 1);
    i16 hi_nibble = -1;
    i16 lo_nibble = -1;
    for (usize i = 0; i < pattern.len; i++)
    {
        if (pattern.ptr[i] == '?')
        {
            *push_array(arena, i16, 1) = -1;
            ++result->len;
        }
        else if (ascii_is_xdigit(pattern.ptr[i]))
        {
            if (hi_nibble == -1)
            {
                hi_nibble = hex_to_u8(pattern.ptr[i]);
            }
            else
            {
                lo_nibble = hex_to_u8(pattern.ptr[i]);
                *push_array(arena, i16, 1) = (hi_nibble << 4) | lo_nibble;
                ++result->len;
                hi_nibble = -1;
                lo_nibble = -1;
            }
        }
    }
    return result;
}

BytesPattern *bytes_pattern_from_str(Arena *arena, Str str)
{
    BytesPattern *result = push_array(arena, BytesPattern, 1);
    result->len = str.len;
    for (usize i = 0; i < str.len; i++)
    {
        result->bytes[i] = str.ptr[i];
    }
    return result;
}

u8 *bytes_scan(Bytes bytes, Str pattern)
{
    Temp scratch = scratch_begin(0, 0);
    u8 *result = bytes_scan_pattern(bytes, make_bytes_pattern(scratch.arena, pattern));
    scratch_end(scratch);
    return result;
}

u8 *bytes_scan_pattern(Bytes bytes, BytesPattern *pattern)
{
    u8 *result = 0;
    for (usize i = 0; i < bytes.len; i++)
    {
        bool match = true;
        for (usize j = 0; j < pattern->len; j++)
        {
            if (pattern->bytes[j] != -1 && bytes.ptr[i + j] != pattern->bytes[j])
            {
                match = false;
                break;
            }
        }
        if (match)
        {
            result = bytes.ptr + i;
            break;
        }
    }
    return result;
}