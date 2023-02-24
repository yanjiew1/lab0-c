/*
 *  Written by David Blackman and Sebastiano Vigna (vigna@acm.org)
 *  Modified by Yan-Jie Wang (yanjiewtw@gmail.com)
 *
 *  To the extent possible under law, the author has dedicated all copyright
 *  and related and neighboring rights to this software to the public domain
 *  worldwide. This software is distributed without any warranty.
 *
 *  See <http://creativecommons.org/publicdomain/zero/1.0/>.
 */

/*
 * Imported from https://prng.di.unimi.it/splitmix64.c
 * and https://prng.di.unimi.it/xoshiro256plusplus.c
 */

#include "xoshiro.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

static uint64_t splitmix64_s = 0;

static void splitmix64_seed(const uint64_t seed)
{
    splitmix64_s = seed;
}

static uint64_t splitmix64()
{
    uint64_t z = (splitmix64_s += 0x9e3779b97f4a7c15);
    z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;
    z = (z ^ (z >> 27)) * 0x94d049bb133111eb;
    return z ^ (z >> 31);
}

static inline uint64_t rotl(const uint64_t x, int k)
{
    return (x << k) | (x >> (64 - k));
}

static uint64_t s[4];

void xoshiro_seed(const uint64_t seed)
{
    splitmix64_seed(seed);
    for (int i = 0; i < 4; i++)
        s[i] = splitmix64();
}

uint64_t xoshiro(void)
{
    static bool seeded = false;
    if (!seeded) {
        xoshiro_seed(0);
        seeded = true;
    }

    const uint64_t result = rotl(s[0] + s[3], 23) + s[0];
    const uint64_t t = s[1] << 17;
    s[2] ^= s[0];
    s[3] ^= s[1];
    s[1] ^= s[2];
    s[0] ^= s[3];
    s[2] ^= t;
    s[3] = rotl(s[3], 45);
    return result;
}

void xoshiro_bytes(uint8_t *dest, const size_t len)
{
    uint64_t tmp;
    for (size_t i = 0; i < len; i += 8) {
        tmp = xoshiro();
        memcpy(&dest[i], &tmp, 8);
    }

    if (len % 8) {
        tmp = xoshiro();
        memcpy(&dest[len - (len % 8)], &tmp, len % 8);
    }
}
