#pragma once

#include "utl/type.h"

#define mem_copy(dst, src, size) memmove((dst), (src), (size))
#define mem_set(dst, byte, size) memset((dst), (byte), (size))
#define mem_zero(dst, size) memset((dst), 0, (size))
#define mem_zero_struct(s) mem_zero((s), sizeof(*(s)))
#define mem_cmp(a, b, size) memcmp((a), (b), (size))
#define mem_eq(a, b, size) (mem_cmp((a), (b), (size)) == 0)

void *mem_reserve(usize size);
bool mem_commit(void *buf, usize size);
void mem_decommit(void *buf, usize size);
void mem_release(void *buf, usize size);