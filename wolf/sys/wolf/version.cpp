#include "version.h"
#include <wolf.hpp>

int w_sys_version(_Inout_z_ char* p_buf, _In_ size_t p_buf_len)
{
    if (p_buf == nullptr || p_buf_len == 0)
    {
        return -1;
    }

    // Making incompatible API changes
    constexpr auto WOLF_MAJOR_VERSION = 3;
    // Adding functionality in a backwards - compatible manner
    constexpr auto WOLF_MINOR_VERSION = 1;
    // bug fixes
    constexpr auto WOLF_PATCH_VERSION = 0;
    // for debugging
    constexpr auto WOLF_DEBUG_VERSION = 0;

#if defined(_WIN32)
    (void)sprintf_s(p_buf, p_buf_len, "%d.%d.%d.%d", WOLF_MAJOR_VERSION,
        WOLF_MINOR_VERSION, WOLF_PATCH_VERSION, WOLF_DEBUG_VERSION);
#else
    (void)sprintf(p_buf, "%d.%d.%d.%d", WOLF_MAJOR_VERSION, WOLF_MINOR_VERSION,
        WOLF_PATCH_VERSION, WOLF_DEBUG_VERSION);
#endif

    return 0;
}
