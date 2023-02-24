#ifndef _XOSHIRO_H
#define _XOSHIRO_H

#include <stddef.h>
#include <stdint.h>

void xoshiro_bytes(uint8_t *dest, const size_t len);
void xoshiro_seed(const uint64_t seed);
uint64_t xoshiro(void);

#endif
