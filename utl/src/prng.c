#include "utl/macro.h"
#include "utl/const.h"
#include "utl/prng.h"

#define rotl(x, k) (((x) << (k)) | ((x) >> (64 - (k))))

static u64 splitmix64(u64 *state)
{
    u64 z = *state += 0x9E3779B97F4A7C15;
    z = (z ^ z >> 30) * 0xBF58476D1CE4E5B9;
    z = (z ^ z >> 27) * 0x94D049BB133111EB;
    return z ^ z >> 31;
}

Prng prng_seed(u64 seed)
{
    Prng rng;
    for (usize i = 0; i < array_count(rng.s); i++)
    {
        rng.s[i] = splitmix64(&seed);
    }
    return rng;
}

// xoshiro256++ PRNG
u64 prng_next(Prng *rng)
{
    u64 *s = rng->s;
    u64 result = rotl(s[0] + s[3], 23) + s[0];
    u64 t = s[1] << 17;
    s[2] ^= s[0];
    s[3] ^= s[1];
    s[1] ^= s[2];
    s[0] ^= s[3];
    s[2] ^= t;
    s[3] = rotl(s[3], 45);
    return result;
}

u64 prng_between(Prng *rng, u64 min, u64 max)
{
    u64 range = max - min;
    u64 value = prng_next(rng);
    return min + value % range;
}

f64 prng_next_f64(Prng *rng)
{
    u64 value = prng_next(rng);
    return (f64)value / (f64)max_u64;
}

f64 prng_between_f64(Prng *rng, f64 min, f64 max)
{
    f64 range = max - min;
    f64 value = prng_next_f64(rng);
    return min + value * range;
}