#ifndef _MEASURE_H_
#define _MEASURE_H_

#include <stdbool.h>

/* Number of measurements per test */
#define N_MEASURES 1500

/* Allow random number range from 0 to 65535 */
#define CHUNK_SIZE 640

#define GEN_DECLARE(op) bool is_##op##_const()


GEN_DECLARE(insert_head);
GEN_DECLARE(insert_tail);
GEN_DECLARE(remove_head);
GEN_DECLARE(remove_tail);

#endif
