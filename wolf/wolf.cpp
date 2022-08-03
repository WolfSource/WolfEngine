#include "wolf.hpp"

auto wolf::w_version() -> std::string 
{
    // Making incompatible API changes
    constexpr auto WOLF_MAJOR_VERSION = 3;
    // Adding functionality in a backwards - compatible manner
    constexpr auto WOLF_MINOR_VERSION = 0;
    // bug fixes
    constexpr auto WOLF_PATCH_VERSION = 0;
    // for debugging
    constexpr auto WOLF_DEBUG_VERSION = 0;

    std::stringstream version;
    version << "v" << WOLF_MAJOR_VERSION << "." << WOLF_MINOR_VERSION << "."
        << WOLF_PATCH_VERSION << "." << WOLF_DEBUG_VERSION;
    return version.str();
}