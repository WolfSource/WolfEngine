/*
	Project: Wolf Engine. Copyright © 2014-2022 Pooya Eimandar
	https://github.com/WolfEngine/WolfEngine
*/
module;

#include "wolf.hpp"
#include <cstdio>

export module wolf;
export import wolf.stream;
export import wolf.system;

namespace wolf {

/**
 * returns wolf version
 * @return string format with the following style
 * "<major>.<minor>.<patch>.<debug>"
 */
C_EXPORT
auto w_sys_version(char *p_buf, size_t p_len) -> void {

  // Making incompatible API changes
  constexpr auto WOLF_MAJOR_VERSION = 3;
  // Adding functionality in a backwards - compatible manner
  constexpr auto WOLF_MINOR_VERSION = 0;
  // bug fixes
  constexpr auto WOLF_PATCH_VERSION = 0;
  // for debugging
  constexpr auto WOLF_DEBUG_VERSION = 0;

  (void)snprintf(p_buf, p_len, "v%d.%d.%d.%d", WOLF_MAJOR_VERSION,
            WOLF_MINOR_VERSION, WOLF_PATCH_VERSION, WOLF_DEBUG_VERSION);
}

} // namespace wolf
