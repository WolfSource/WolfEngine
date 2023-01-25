#include <emscripten/emscripten.h>
#include <thread>
#include <wolf/stream/janus/w_janus_api_emc.hpp>
#include <wolf/wolf.hpp>

using w_janus_api_emc = wolf::stream::janus::w_janus_api_emc;

int main() {
  using namespace std::chrono_literals;

  const auto _version = "Wolf" + wolf::w_init();
  emscripten_log(EM_LOG_CONSOLE, _version.c_str());

  constexpr auto _debug = 1;
  w_janus_api_emc::init(_debug, [](int p_init_state) {
    if (p_init_state != 0) {
      return;
    }
    w_janus_api_emc::share_screen(
        "pooya", "screensharing test", 500000, [](_In_ double p_room_id) {
          const auto _room_id_str =
              "room id is: " +
              std::to_string(gsl::narrow_cast<uint64_t>(p_room_id));
          emscripten_log(EM_LOG_CONSOLE, _room_id_str.c_str());
        });
  });

  return 0;
}