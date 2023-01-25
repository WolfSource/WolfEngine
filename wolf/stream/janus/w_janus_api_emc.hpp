/*
    Project: Wolf Engine. Copyright © 2014-2023 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#if defined(WOLF_STREAM_JANUS) && defined(EMSCRIPTEN)

#pragma once

#include <list>
#include <wolf/wolf.hpp>

namespace wolf::stream::janus {
class w_janus_api_emc {
public:
  // Initialize the library
  W_API static boost::leaf::result<int>
  init(_In_ int p_debug_mode,
       _In_ std::function<void(int)> p_on_init_callback) noexcept;

  W_API static boost::leaf::result<int>
  share_screen(_In_ const char *p_username, _In_ const char *p_room_desc,
               _In_ int p_bitrate,
               _In_ std::function<void(double)> p_on_success) noexcept;
};
} // namespace wolf::stream::janus

#endif // defined(WOLF_STREAM_JANUS) AND defined(EMSCRIPTEN)