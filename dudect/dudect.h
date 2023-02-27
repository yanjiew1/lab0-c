/*
 dudect: dude, is my code constant time?
 https://github.com/oreparaz/dudect
 oscar.reparaz@esat.kuleuven.be

 Based on the following paper:

     Oscar Reparaz, Josep Balasch and Ingrid Verbauwhede
     dude, is my code constant time?
     DATE 2017
     https://eprint.iacr.org/2016/1123.pdf

 This file measures the execution time of a given function many times with
 different inputs and performs a Welch's t-test to determine if the function
 runs in constant time or not. This is essentially leakage detection, and
 not a timing attack.

 Notes:

   - the execution time distribution tends to be skewed towards large
     timings, leading to a fat right tail. Most executions take little time,
     some of them take a lot. We try to speed up the test process by
     throwing away those measurements with large cycle count. (For example,
     those measurements could correspond to the execution being interrupted
     by the OS.) Setting a threshold value for this is not obvious; we just
     keep the x% percent fastest timings, and repeat for several values of x.

   - the previous observation is highly heuristic. We also keep the uncropped
     measurement time and do a t-test on that.

   - we also test for unequal variances (second order test), but this is
     probably redundant since we're doing as well a t-test on cropped
     measurements (non-linear transform)

   - as long as any of the different test fails, the code will be deemed
     variable time.

 LICENSE:

    This is free and unencumbered software released into the public domain.
    Anyone is free to copy, modify, publish, use, compile, sell, or
    distribute this software, either in source code form or as a compiled
    binary, for any purpose, commercial or non-commercial, and by any
    means.
    In jurisdictions that recognize copyright laws, the author or authors
    of this software dedicate any and all copyright interest in the
    software to the public domain. We make this dedication for the benefit
    of the public at large and to the detriment of our heirs and
    successors. We intend this dedication to be an overt act of
    relinquishment in perpetuity of all present and future rights to this
    software under copyright law.
    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
    IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
    OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
    ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
    OTHER DEALINGS IN THE SOFTWARE.
    For more information, please refer to <http://unlicense.org>
*/


#ifndef DUDECT_H_INCLUDED
#define DUDECT_H_INCLUDED

#include <stddef.h>
#include <stdint.h>

#ifdef DUDECT_VISIBLITY_STATIC
#define DUDECT_VISIBILITY static
#else
#define DUDECT_VISIBILITY extern
#endif

/*
   The interface of dudect begins here, to be compiled only if
   DUDECT_IMPLEMENTATION is not defined. In a multi-file library what follows
   would be the public-facing dudect.h
*/

#define DUDECT_ENOUGH_MEASUREMENTS                                  \
    (10000) /* do not draw any conclusion before we reach this many \
               measurements */
#define DUDECT_NUMBER_PERCENTILES (100)

/* perform this many tests in total:
   - 1 first order uncropped test,
   - DUDECT_NUMBER_PERCENTILES tests
   - 1 second order test
*/
#define DUDECT_TESTS (1 + DUDECT_NUMBER_PERCENTILES + 1)

typedef struct __dudect_config dudect_config_t;

typedef void (*dudect_prepare_func_t)(void *priv,
                                      dudect_config_t *conf,
                                      uint8_t *input_data,
                                      uint8_t *classes);

typedef uint8_t (*dudect_compute_func_t)(void *priv,
                                         size_t size,
                                         uint8_t *data);

struct __dudect_config {
    size_t chunk_size;
    size_t number_measurements;
    void *priv;
    dudect_prepare_func_t prepare;
    dudect_compute_func_t compute;
};

typedef struct {
    double mean[2];
    double m2[2];
    double n[2];
} ttest_ctx_t;

typedef struct {
    int64_t *ticks;
    int64_t *exec_times;
    uint8_t *input_data;
    uint8_t *classes;
    dudect_config_t *config;
    ttest_ctx_t *ttest_ctxs[DUDECT_TESTS];
    int64_t *percentiles;
} dudect_ctx_t;

typedef enum {
    DUDECT_LEAKAGE_FOUND = 0,
    DUDECT_NO_LEAKAGE_EVIDENCE_YET,
    DUDECT_NOT_ENOUGHT_MEASUREMENTS,
} dudect_state_t;

/* Public API */

DUDECT_VISIBILITY int dudect_init(dudect_ctx_t *ctx, dudect_config_t *conf);
DUDECT_VISIBILITY dudect_state_t dudect_main(dudect_ctx_t *c);
DUDECT_VISIBILITY int dudect_free(dudect_ctx_t *ctx);

#endif /* DUDECT_H_INCLUDED */
