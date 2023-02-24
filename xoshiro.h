#ifndef _XOSHIRO_H
#define _XOSHIRO_H

#include <stdint.h>

void xoshiro_seed(const uint64_t seed);
uint64_t xoshiro(void);

#endif
