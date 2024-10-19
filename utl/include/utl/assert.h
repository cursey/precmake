#pragma once

#include "utl/whoami.h"

#if OS_WINDOWS
#define assert_trap() __debugbreak()
#else
#define assert_trap() __builtin_trap()
#endif

#define assert_always(expr)                                                                                            \
    do                                                                                                                 \
    {                                                                                                                  \
        if (!(expr))                                                                                                   \
        {                                                                                                              \
            assert_trap();                                                                                             \
        }                                                                                                              \
    } while (0)

#if BUILD_DEBUG
#define assert_debug(expr) assert_always(expr)
#else
#define assert_debug(expr)
#endif
