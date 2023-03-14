extern "C" {
#include "wolf.h"
}
#include <catch2/catch_all.hpp>

TEST_CASE("wolf"){
  int major = 0, minor = 0, patch = 0;
  w_version(&major, &minor, &patch);
  REQUIRE(major == 3);
}