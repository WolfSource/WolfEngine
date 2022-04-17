#include "wolf.h"
#include <cstdio>
#include <cstring>

constexpr auto WOLF_CC_MAJOR_VERSION = 3; // Making incompatible API changes
constexpr auto WOLF_CC_MINOR_VERSION = 0; // Adding functionality in a backwards - compatible manner
constexpr auto WOLF_CC_PATCH_VERSION = 3; // bug fixes
constexpr auto WOLF_CC_DEBUG_VERSION = 0; // for debugging

int wolf_init(void)
{
    return 0;
}

void wolf_version(char *p_version)
{
    constexpr auto _max_size = 12;
    auto _len = strlen(p_version);
    if (_len > _max_size)
    {
        sprintf(p_version, "v%d.%d.%d.%d",
                WOLF_CC_MAJOR_VERSION,
                WOLF_CC_MINOR_VERSION,
                WOLF_CC_PATCH_VERSION,
                WOLF_CC_DEBUG_VERSION);
    }
}

int wolf_fini(void)
{
    return 0;
}