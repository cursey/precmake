#include <stdio.h>
#include <stdlib.h>
#include "utl/bytes.h"
#include "utl/str.h"

bool ascii_is_digit(u8 ch)
{
    return ch >= '0' && ch <= '9';
}

bool ascii_is_xdigit(u8 ch)
{
    return (ch >= '0' && ch <= '9') || (ch >= 'A' && ch <= 'F') || (ch >= 'a' && ch <= 'f');
}

bool ascii_is_whitespace(u8 c)
{
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

bool ascii_is_upper(u8 c)
{
    return c >= 'A' && c <= 'Z';
}

bool ascii_is_lower(u8 c)
{
    return c >= 'a' && c <= 'z';
}

u8 ascii_to_upper(u8 c)
{
    if (ascii_is_lower(c))
    {
        c += 'A' - 'a';
    }
    return c;
}

u8 ascii_to_lower(u8 c)
{
    if (ascii_is_upper(c))
    {
        c += 'a' - 'A';
    }
    return c;
}

usize cstr_len(u8 *str)
{
    u8 *s = str;
    for (; *s != 0; s++)
    {
    }
    return s - str;
}

usize cstr16_len(u16 *str)
{
    u16 *s = str;
    for (; *s != 0; s++)
    {
    }
    return s - str;
}

usize cstr32_len(u32 *str)
{
    u32 *s = str;
    for (; *s != 0; s++)
    {
    }
    return s - str;
}

Str make_str(u8 *str, usize len)
{
    return (Str){.ptr = str, .len = len};
}

Str16 make_str16(u16 *str, usize len)
{
    return (Str16){.ptr = str, .len = len};
}

Str32 make_str32(u32 *str, usize len)
{
    return (Str32){.ptr = str, .len = len};
}

Str str_from_cstr(u8 *str)
{
    return make_str(str, cstr_len(str));
}

Str16 str16_from_cstr(u16 *str)
{
    return make_str16(str, cstr16_len(str));
}

Str32 str32_from_cstr(u32 *str)
{
    return make_str32(str, cstr32_len(str));
}

i64 i64_from_str_base(Str str, i32 base)
{
    // Skip leading whitespace.
    while (str.len > 0 && ascii_is_whitespace(*str.ptr))
    {
        str.ptr++;
        str.len--;
    }

    // Copy the string to a buffer.
    char buf[64];
    usize len = n_min(str.len, sizeof(buf) - 1);
    mem_copy(buf, str.ptr, len);
    buf[len] = 0;

    // Convert the string to an integer.
    char *end;
    return strtol(buf, &end, base);
}

i64 i64_from_str(Str str)
{
    return i64_from_str_base(str, 10);
}

f64 f64_from_str(Str str)
{
    // Skip leading whitespace.
    while (str.len > 0 && ascii_is_whitespace(*str.ptr))
    {
        str.ptr++;
        str.len--;
    }

    // Copy the string to a buffer.
    char buf[64];
    usize len = n_min(str.len, sizeof(buf) - 1);
    mem_copy(buf, str.ptr, len);
    buf[len] = 0;

    // Convert the string to a float.
    char *end;
    return strtod(buf, &end);
}

Str push_str_copy(Arena *arena, Str str)
{
    Str copy = {.ptr = push_array_no_zero(arena, u8, str.len + 1), .len = str.len};
    mem_copy(copy.ptr, str.ptr, str.len);
    copy.ptr[str.len] = 0;
    return copy;
}

Str push_str_cat(Arena *arena, Str a, Str b)
{
    Str str;
    str.len = a.len + b.len;
    str.ptr = push_array_no_zero(arena, u8, str.len + 1);
    mem_copy(str.ptr, a.ptr, a.len * sizeof(u8));
    mem_copy(str.ptr + a.len, b.ptr, b.len * sizeof(u8));
    str.ptr[str.len] = 0;
    return str;
}

Str push_strfv(Arena *arena, char *fmt, va_list args)
{
    va_list args2;
    va_copy(args2, args);
    u32 needed_bytes = vsnprintf(0, 0, fmt, args) + 1;
    Str result = {0};
    result.ptr = push_array_no_zero(arena, u8, needed_bytes);
    result.len = vsnprintf((char *)result.ptr, needed_bytes, fmt, args2);
    result.ptr[result.len] = 0;
    va_end(args2);
    return result;
}

Str push_strf(Arena *arena, char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    Str result = push_strfv(arena, fmt, args);
    va_end(args);
    return result;
}

StrNode *str_list_push_node(StrList *list, StrNode *node)
{
    queue_push(list->head, list->tail, node);
    ++list->node_count;
    list->total_len += node->str.len;
    return node;
}

StrNode *str_list_push_node_set_str(StrList *list, StrNode *node, Str str)
{
    queue_push(list->head, list->tail, node);
    ++list->node_count;
    list->total_len += str.len;
    node->str = str;
    return node;
}

StrNode *str_list_push(Arena *arena, StrList *list, Str str)
{
    StrNode *node = push_array_no_zero(arena, StrNode, 1);
    str_list_push_node_set_str(list, node, str);
    return node;
}

Str str_list_flatten(Arena *arena, StrList *list)
{
    Str result;
    result.ptr = push_array(arena, u8, list->total_len + 1);
    result.len = 0;
    for (StrNode *node = list->head; node != 0; node = node->next)
    {
        mem_copy(result.ptr + result.len, node->str.ptr, node->str.len);
        result.len += node->str.len;
    }
    return result;
}

i8 str_cmp(Str a, Str b)
{
    usize len = n_min(a.len, b.len);
    for (usize i = 0; i < len; i++)
    {
        u8 x = a.ptr[i];
        u8 y = b.ptr[i];
        if (x != y)
        {
            return x < y ? -1 : 1;
        }
    }
    if (a.len == b.len)
    {
        return 0;
    }
    return a.len < b.len ? -1 : 1;
}

i8 str_cmp_i(Str a, Str b)
{
    usize len = n_min(a.len, b.len);
    for (usize i = 0; i < len; i++)
    {
        u8 x = ascii_to_lower(a.ptr[i]);
        u8 y = ascii_to_lower(b.ptr[i]);
        if (x != y)
        {
            return x < y ? -1 : 1;
        }
    }
    if (a.len == b.len)
    {
        return 0;
    }
    return a.len < b.len ? -1 : 1;
}

bool str_eq(Str a, Str b)
{
    return str_cmp(a, b) == 0;
}

bool str_eq_i(Str a, Str b)
{
    return str_cmp_i(a, b) == 0;
}

bool str_starts_with(Str a, Str b)
{
    if (b.len > a.len)
    {
        return false;
    }
    for (usize i = 0; i < b.len; i++)
    {
        if (a.ptr[i] != b.ptr[i])
        {
            return false;
        }
    }
    return true;
}

bool str_starts_with_i(Str a, Str b)
{
    if (b.len > a.len)
    {
        return false;
    }
    for (usize i = 0; i < b.len; i++)
    {
        u8 a_lower = ascii_to_lower(a.ptr[i]);
        u8 b_lower = ascii_to_lower(b.ptr[i]);
        if (a_lower != b_lower)
        {
            return false;
        }
    }
    return true;
}

bool str_ends_with(Str a, Str b)
{
    if (b.len > a.len)
    {
        return false;
    }
    for (usize i = 0; i < b.len; i++)
    {
        if (a.ptr[a.len - b.len + i] != b.ptr[i])
        {
            return false;
        }
    }
    return true;
}

bool str_ends_with_i(Str a, Str b)
{
    if (b.len > a.len)
    {
        return false;
    }
    for (usize i = 0; i < b.len; i++)
    {
        u8 a_lower = ascii_to_lower(a.ptr[a.len - b.len + i]);
        u8 b_lower = ascii_to_lower(b.ptr[i]);
        if (a_lower != b_lower)
        {
            return false;
        }
    }
    return true;
}

Str str_trim_start(Str str, Str chars)
{
    while (str.len > 0)
    {
        bool found = false;
        for (usize i = 0; i < chars.len; i++)
        {
            if (str.ptr[0] == chars.ptr[i])
            {
                str.ptr++;
                str.len--;
                found = true;
                break;
            }
        }
        if (!found)
        {
            break;
        }
    }
    return str;
}

Str str_trim_end(Str str, Str chars)
{
    while (str.len > 0)
    {
        bool found = false;
        for (usize i = 0; i < chars.len; i++)
        {
            if (str.ptr[str.len - 1] == chars.ptr[i])
            {
                str.len--;
                found = true;
                break;
            }
        }
        if (!found)
        {
            break;
        }
    }
    return str;
}

Str str_trim(Str str, Str chars)
{
    return str_trim_start(str_trim_end(str, chars), chars);
}

Str str_escape(Arena *arena, Str str)
{
    Str result = {0};
    result.ptr = push_array(arena, u8, str.len * 2);
    result.len = 0;
    for (usize i = 0; i < str.len; i++)
    {
        switch (str.ptr[i])
        {
        case '\n':
            result.ptr[result.len++] = '\\';
            result.ptr[result.len++] = 'n';
            break;
        case '\r':
            result.ptr[result.len++] = '\\';
            result.ptr[result.len++] = 'r';
            break;
        case '\t':
            result.ptr[result.len++] = '\\';
            result.ptr[result.len++] = 't';
            break;
        case '\\':
            result.ptr[result.len++] = '\\';
            result.ptr[result.len++] = '\\';
            break;
        default:
            result.ptr[result.len++] = str.ptr[i];
            break;
        }
    }
    arena_pop(arena, str.len * 2 - result.len);
    return result;
}

Str str_unescape(Arena *arena, Str str)
{
    Str result = {0};
    result.ptr = push_array(arena, u8, str.len);
    result.len = 0;
    for (usize i = 0; i < str.len; i++)
    {
        if (str.ptr[i] == '\\' && i + 1 < str.len)
        {
            switch (str.ptr[i + 1])
            {
            case 'n':
                result.ptr[result.len++] = '\n';
                i++;
                break;
            case 'r':
                result.ptr[result.len++] = '\r';
                i++;
                break;
            case 't':
                result.ptr[result.len++] = '\t';
                i++;
                break;
            case '\\':
                result.ptr[result.len++] = '\\';
                i++;
                break;
            default:
                result.ptr[result.len++] = str.ptr[i];
                break;
            }
        }
        else
        {
            result.ptr[result.len++] = str.ptr[i];
        }
    }
    arena_pop(arena, str.len - result.len);
    return result;
}

usize str_find(Str str, Str sub)
{
    for (usize i = 0; i < str.len; i++)
    {
        if (str_starts_with(make_str(str.ptr + i, str.len - i), sub))
        {
            return i;
        }
    }
    return str.len;
}

usize str_find_i(Str str, Str sub)
{
    for (usize i = 0; i < str.len; i++)
    {
        if (str_starts_with_i(make_str(str.ptr + i, str.len - i), sub))
        {
            return i;
        }
    }
    return str.len;
}

usize str_rfind(Str str, Str sub)
{
    for (isize i = str.len; i >= 0; i--)
    {
        if (str_starts_with(make_str(str.ptr + i, str.len - i), sub))
        {
            return i;
        }
    }
    return str.len;
}

usize str_rfind_i(Str str, Str sub)
{
    for (isize i = str.len; i >= 0; i--)
    {
        if (str_starts_with_i(make_str(str.ptr + i, str.len - i), sub))
        {
            return i;
        }
    }
    return str.len;
}

Str str_substr(Str str, usize start, isize len)
{
    if (len < 0)
    {
        len = str.len - start;
    }
    Str result = {
        .ptr = str.ptr + start,
        .len = n_min(len, str.len - start),
    };
    return result;
}