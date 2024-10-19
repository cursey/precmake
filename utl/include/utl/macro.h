#pragma once

#include <string.h>
#include "utl/type.h"
#include "utl/whoami.h"

#define kilobyte(n) (((usize)(n)) << 10)
#define megabyte(n) (((usize)(n)) << 20)
#define gigabyte(n) (((usize)(n)) << 30)
#define terabyte(n) (((usize)(n)) << 40)
#define thousand(n) ((n) * 1000)
#define million(n) ((n) * 1000000)
#define billion(n) ((n) * 1000000000)

#define n_max(a, b) ((a) > (b) ? (a) : (b))
#define n_min(a, b) ((a) < (b) ? (a) : (b))

#define array_count(arr) (sizeof(arr) / sizeof((arr)[0]))

#if COMPILER_MSVC || COMPILER_CLANG
#define align_of(T) __alignof(T)
#elif COMPILER_GCC
#define align_of(T) __alignof__(T)
#endif

#define align_up(n, alignment) (((n) + (alignment)-1) & (~((alignment)-1)))
#define align_down(n, alignment) ((n) & (~((alignment)-1)))

#if COMPILER_MSVC
#define thread_alloc __declspec(thread)
#elif COMPILER_CLANG || COMPILER_GCC
#define thread_alloc __thread
#endif

#define offset_of(T, member) ((usize)(&((T *)0)->member))

#define stack_push_next(front, node, next)                                                                             \
    do                                                                                                                 \
    {                                                                                                                  \
        (node)->next = (front);                                                                                        \
        (front) = (node);                                                                                              \
    } while (0)

#define stack_pop_next(front, next)                                                                                    \
    do                                                                                                                 \
    {                                                                                                                  \
        (front) = (front)->next;                                                                                       \
    } while (0)

#define stack_push(front, node) stack_push_next(front, node, next)
#define stack_pop(front) stack_pop_next(front, next)

#define queue_push_next(head, tail, node, next)                                                                        \
    do                                                                                                                 \
    {                                                                                                                  \
        if ((head) == 0)                                                                                               \
        {                                                                                                              \
            (head) = (node);                                                                                           \
        }                                                                                                              \
        else                                                                                                           \
        {                                                                                                              \
            (tail)->next = (node);                                                                                     \
        }                                                                                                              \
        (tail) = (node);                                                                                               \
        (node)->next = 0;                                                                                              \
    } while (0)

#define queue_pop_next(head, tail, next)                                                                               \
    do                                                                                                                 \
    {                                                                                                                  \
        (head) = (head)->next;                                                                                         \
        if ((head) == 0)                                                                                               \
        {                                                                                                              \
            (tail) = 0;                                                                                                \
        }                                                                                                              \
    } while (0)

#define queue_push(head, tail, node) queue_push_next(head, tail, node, next)
#define queue_pop(head, tail) queue_pop_next(head, tail, next)
