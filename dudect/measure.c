#include "measure.h"
#include "dudect.h"
#include "queue.h"
#include "random.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEST_TRIES 10

/* Maintain a queue independent from the qtest since
 * we do not want the test to affect the original functionality
 */
static struct list_head *l = NULL;

static uint8_t compute_insert_head(void *priv, size_t size, uint8_t *data)
{
    q_insert_head(l, (char *) data);
    return 0;
}

static uint8_t compute_insert_tail(void *priv, size_t size, uint8_t *data)
{
    q_insert_tail(l, (char *) data);
    return 0;
}

static uint8_t compute_remove_head(void *priv, size_t size, uint8_t *data)
{
    element_t *elem = q_remove_head(l, NULL, 0);
    q_release_element(elem);
    return 0;
}

static uint8_t compute_remove_tail(void *priv, size_t size, uint8_t *data)
{
    element_t *elem = q_remove_tail(l, NULL, 0);
    q_release_element(elem);
    return 0;
}

static void fixed_string(uint8_t *buf, size_t len)
{
    memset(buf, 'a', len - 1);
    buf[len - 1] = '\0';
}

static void random_string(uint8_t *buf, size_t len)
{
    uint8_t rand;
    for (size_t i = 0; i < len - 1; i++) {
        randombytes(&rand, 1);
        buf[i] = 'a' + (rand % 26);
    }
    buf[len - 1] = '\0';
}

static void prepare_inserts(void *priv,
                            dudect_config_t *cfg,
                            uint8_t *input_data,
                            uint8_t *classes)
{
    for (size_t i = 0; i < N_MEASURES; i++) {
        uint8_t *ptr = input_data + i * CHUNK_SIZE;
        classes[i] = randombit();
        if (classes[i] == 0)
            fixed_string(ptr, CHUNK_SIZE);
        else
            random_string(ptr, CHUNK_SIZE);
    }
}

static void prepare_insert_head(void *priv,
                                dudect_config_t *cfg,
                                uint8_t *input_data,
                                uint8_t *classes)
{
    prepare_inserts(priv, cfg, input_data, classes);
}

static void prepare_insert_tail(void *priv,
                                dudect_config_t *cfg,
                                uint8_t *input_data,
                                uint8_t *classes)
{
    prepare_inserts(priv, cfg, input_data, classes);
}

static void prepare_remove_head(void *priv,
                                dudect_config_t *cfg,
                                uint8_t *input_data,
                                uint8_t *classes)
{
    uint8_t tmp[CHUNK_SIZE];
    for (int i = 0; i < N_MEASURES; i++) {
        classes[i] = randombit();
        if (classes[i] == 0)
            fixed_string(tmp, CHUNK_SIZE);
        else
            random_string(tmp, CHUNK_SIZE);
        q_insert_tail(l, (char *) tmp);
    }
}

static void prepare_remove_tail(void *priv,
                                dudect_config_t *cfg,
                                uint8_t *input_data,
                                uint8_t *classes)
{
    uint8_t tmp[CHUNK_SIZE];
    for (int i = 0; i < N_MEASURES; i++) {
        classes[i] = randombit();
        if (classes[i] == 0)
            fixed_string(tmp, CHUNK_SIZE);
        else
            random_string(tmp, CHUNK_SIZE);
        q_insert_head(l, (char *) tmp);
    }
}

#define GEN_DUDECT_CONFIG(op)                                       \
    static dudect_config_t config_##op = {.prepare = prepare_##op,  \
                                          .compute = compute_##op,  \
                                          .priv = NULL,             \
                                          .chunk_size = CHUNK_SIZE, \
                                          .number_measurements = N_MEASURES}

GEN_DUDECT_CONFIG(insert_head);
GEN_DUDECT_CONFIG(insert_tail);
GEN_DUDECT_CONFIG(remove_head);
GEN_DUDECT_CONFIG(remove_tail);

#define GEN_TEST_FUNC(op)                                           \
    bool is_##op##_const()                                          \
    {                                                               \
        for (int i = 0; i < TEST_TRIES; i++) {                      \
            printf("Testing %s...(%d/%d)\n\n", #op, i, TEST_TRIES); \
            if (l)                                                  \
                q_free(l);                                          \
            l = q_new();                                            \
            dudect_ctx_t ctx;                                       \
            dudect_init(&ctx, &config_##op);                        \
            dudect_state_t state = DUDECT_NOT_ENOUGHT_MEASUREMENTS; \
            while (state == DUDECT_NOT_ENOUGHT_MEASUREMENTS)        \
                state = dudect_main(&ctx);                          \
            dudect_free(&ctx);                                      \
            q_free(l);                                              \
            l = NULL;                                               \
            printf("\033[A\033[2K\033[A\033[2K");                   \
            if (state == DUDECT_NO_LEAKAGE_EVIDENCE_YET)            \
                return true;                                        \
        }                                                           \
        return false;                                               \
    }

GEN_TEST_FUNC(insert_head)
GEN_TEST_FUNC(insert_tail)
GEN_TEST_FUNC(remove_head)
GEN_TEST_FUNC(remove_tail)
