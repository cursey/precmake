#pragma once

#include "utl/type.h"

typedef struct Prng Prng;
struct Prng
{
    u64 s[4];
};

Prng prng_seed(u64 seed);
u64 prng_next(Prng *prng);
u64 prng_between(Prng *prng, u64 min, u64 max);
f64 prng_next_f64(Prng *prng);
f64 prng_between_f64(Prng *prng, f64 min, f64 max);
