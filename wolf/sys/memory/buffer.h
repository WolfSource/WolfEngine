#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include <wolf.h>

#define BUFFER_ALIGNED_SIZE 16

// aligned and packed of buffer
typedef struct buffer
{
    uint8_t *data;
    size_t size;
} buffer __attribute__((aligned(BUFFER_ALIGNED_SIZE)));

/**
 * allocate buffer
 * @return allocated buffer
 */
WOLF_API buffer *w_malloc(size_t p_size);

/**
 * free memory of buffer
 * @param p_mem is the memory buffer
 */
WOLF_API void w_free(buffer *p_mem);

#ifdef __cplusplus
}
#endif