#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp> // NOLINT

#include <version.h>

#include <iostream>
#include <format>

TEST_CASE("wolf", "[single-file]") { 
    w_sys_version(nullptr, 0); 
}