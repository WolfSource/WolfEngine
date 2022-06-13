#include "version.h"
#include <stdio.h>

void w_sys_version(char *p_buf, size_t p_len) {

  if (p_buf == NULL || p_len == 0) {
    return;
  }

  // Making incompatible API changes
  const int WOLF_MAJOR_VERSION = 3;
  // Adding functionality in a backwards - compatible manner
  const int WOLF_MINOR_VERSION = 0;
  // bug fixes
  const int WOLF_PATCH_VERSION = 0;
  // for debugging
  const int WOLF_DEBUG_VERSION = 0;

  sprintf_s(p_buf, p_len , "v%d.%d.%d.%d", WOLF_MAJOR_VERSION,
               WOLF_MINOR_VERSION, WOLF_PATCH_VERSION, WOLF_DEBUG_VERSION);
}
