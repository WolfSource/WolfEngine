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
  /**
   * initialize the Janus library
   * @param p_debug_mode, enable debug mode
   * @param p_on_init_callback, on init callback
   * @returns 0 on success
   */
  W_API static boost::leaf::result<int>
  init(_In_ bool p_debug_mode,
       _In_ std::function<void(int)> p_on_init_callback) noexcept;

  /**
   * create a Janus session and share current screen
   * @param p_username, the username
   * @param p_room_desc, a description for the room
   * @param p_bitrate, the bitrate of stream
   * @param p_on_success, a callback which will be raised on success
   * @returns 0 on success
   */
  W_API static boost::leaf::result<int> share_screen(
      _In_ const char *p_username, _In_ const char *p_room_desc,
      _In_ int p_bitrate,
      _In_ std::function<void(double /*p_room_id*/)> p_on_success) noexcept;

  /**
   * create a Janus session and share current screen
   * @param p_room_id, an id of room
   * @param p_username, the username
   * @param p_on_success, a callback which will be raised on success
   * @returns 0 on success
   */
  W_API static boost::leaf::result<int>
  join_room(_In_ double p_room_id, _In_ const char *p_username,
            _In_ std::function<void(void)> p_on_success) noexcept;
};
} // namespace wolf::stream::janus

#endif // defined(WOLF_STREAM_JANUS) AND defined(EMSCRIPTEN)