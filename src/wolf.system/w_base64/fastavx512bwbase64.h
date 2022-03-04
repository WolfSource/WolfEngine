#ifndef FASTBASE64_AVX512BW_H_
#define FASTBASE64_AVX512BW_H_

#if defined(WOLF_ENABLE_AVX512) && ((defined(_MSC_VER) && _MSC_VER >= 1911) || (defined(__INTEL_COMPILER) && __INTEL_COMPILER >= 1600) || (defined(__clang__) && __clang_major__ >= 4) || (defined(__GNUC__) && __GNUC__ >= 5))

/**
* Assumes recent x64 hardware with AVX512BW instructions.
*/

#include <stddef.h>
#include <stdint.h>
#include "chromiumbase64.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

size_t fast_avx512bw_base64_decode(char *out, const char *src, size_t srclen);
size_t fast_avx512bw_base64_encode(char* dest, const char* str, size_t len);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

#endif
