#pragma once

#include <stdarg.h>
#include "utl/type.h"
#include "utl/arena.h"

typedef struct Str Str;
struct Str
{
    u8 *ptr;
    usize len;
};

typedef struct Str16 Str16;
struct Str16
{
    u16 *ptr;
    usize len;
};

typedef struct Str32 Str32;
struct Str32
{
    u32 *ptr;
    usize len;
};

typedef struct StrNode StrNode;
struct StrNode
{
    StrNode *next;
    Str str;
};

typedef struct StrList StrList;
struct StrList
{
    StrNode *head;
    StrNode *tail;
    usize node_count;
    usize total_len;
};

bool ascii_is_digit(u8 ch);
bool ascii_is_xdigit(u8 ch);
bool ascii_is_whitespace(u8 ch);
bool ascii_is_upper(u8 ch);
bool ascii_is_lower(u8 ch);
u8 ascii_to_upper(u8 ch);
u8 ascii_to_lower(u8 ch);

usize cstr_len(u8 *str);
usize cstr16_len(u16 *str);
usize cstr32_len(u32 *str);

#define str_lit(S) make_str((u8 *)S, sizeof(S) - 1)
#define str_varg(S) (int)((S).len), (const char *)((S).ptr)

Str make_str(u8 *str, usize len);
Str16 make_str16(u16 *str, usize len);
Str32 make_str32(u32 *str, usize len);
Str str_from_cstr(u8 *str);
Str16 str16_from_cstr(u16 *str);
Str32 str32_from_cstr(u32 *str);

i64 i64_from_str_base(Str str, i32 base);
i64 i64_from_str(Str str);
f64 f64_from_str(Str str);

Str push_str_copy(Arena *arena, Str str);
Str push_str_cat(Arena *arena, Str a, Str b);
Str push_strfv(Arena *arena, char *fmt, va_list args);
Str push_strf(Arena *arena, char *fmt, ...);

StrNode *str_list_push_node(StrList *list, StrNode *node);
StrNode *str_list_push_node_set_str(StrList *list, StrNode *node, Str str);
StrNode *str_list_push(Arena *arena, StrList *list, Str str);
Str str_list_flatten(Arena *arena, StrList *list);

i8 str_cmp(Str a, Str b);
i8 str_cmp_i(Str a, Str b);
bool str_eq(Str a, Str b);
bool str_eq_i(Str a, Str b);
bool str_starts_with(Str a, Str b);
bool str_starts_with_i(Str a, Str b);
bool str_ends_with(Str a, Str b);
bool str_ends_with_i(Str a, Str b);

Str str_trim_start(Str str, Str chars);
Str str_trim_end(Str str, Str chars);
Str str_trim(Str str, Str chars);

Str str_escape(Arena *arena, Str str);
Str str_unescape(Arena *arena, Str str);

usize str_find(Str str, Str sub);
usize str_find_i(Str str, Str sub);
usize str_rfind(Str str, Str sub);
usize str_rfind_i(Str str, Str sub);

Str str_substr(Str str, usize start, isize len);
