#ifndef _QRANDOM_H
#define _QRANDOM_H

#include <stddef.h>
#include <stdint.h>

extern int qrandom_impl;
uint64_t qrandom();
int qrandombytes(uint8_t *buf, size_t len);
int qrandom_int(const int max);

#endif
