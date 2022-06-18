#include "version.h"
#include <cstdio>

void w_sys_version(char *p_buf, size_t p_len) {

  if (p_buf == nullptr || p_len > W_MAX_PATH) {
    return;
  }

  // Making incompatible API changes
  constexpr auto WOLF_MAJOR_VERSION = 3;
  // Adding functionality in a backwards - compatible manner
  constexpr auto WOLF_MINOR_VERSION = 0;
  // bug fixes
  constexpr auto WOLF_PATCH_VERSION = 0;
  // for debugging
  constexpr auto WOLF_DEBUG_VERSION = 0;

  sprintf_s(p_buf, p_len , "v%d.%d.%d.%d", WOLF_MAJOR_VERSION, WOLF_MINOR_VERSION,
               WOLF_PATCH_VERSION, WOLF_DEBUG_VERSION);
}
