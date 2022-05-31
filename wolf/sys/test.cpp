#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp> // NOLINT

import wolf;
import wolf.stream;

#include <iostream>
#include <format>

TEST_CASE("wolf", "[single-file]") {

  using namespace wolf::stream::rist;
  w_rist_ctx_t r{};
  w_rist_receiver_create(&r);
}