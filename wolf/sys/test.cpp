#define CATCH_CONFIG_MAIN
#include "catch.hpp" // NOLINT

#include <wolf.h>
#include <memory/buffer.h>

TEST_CASE("w_memory tests", "[single-file]")
{
    auto a = w_malloc(32);
    w_free(a);
    // w_sys_version();
    //  REQUIRE(_str == "hello"); // NOLINT
}