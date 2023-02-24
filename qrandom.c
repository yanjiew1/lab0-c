#include "qrandom.h"
#include <limits.h>
#include "random.h"
#include "xoshiro.h"

int qrandom_impl = 0;

uint64_t qrandom()
{
    uint64_t tmp;
    if (qrandom_impl == 0) {
        randombytes((uint8_t *) &tmp, sizeof(tmp));
    } else {
        tmp = xoshiro();
    }
    return tmp;
}

int qrandombytes(uint8_t *dest, const size_t len)
{
    if (qrandom_impl == 0) {
        return randombytes(dest, len);
    } else {
        xoshiro_bytes(dest, len);
        return 0;
    }
}

int qrandom_int(const int max)
{
    int n;
    do {
        n = qrandom() & INT_MAX;
    } while (n >= INT_MAX - (INT_MAX % (max + 1)));
    n %= max + 1;

    return n;
}
