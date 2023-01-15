#if defined(WOLF_SYSTEM_HTTP1_WS) && defined(EMSCRIPTEN)

#pragma once

#include <wolf.hpp>

//#include <emscripten/emscripten.h>
//#include <emscripten/websocket.h>
//
//namespace wolf::system::socket {
//class w_ws_client_emc {
//public:
//  // default constructor
//  W_API w_ws_client_emc() noexcept = default;
//
//  // move constructor.
//  W_API w_ws_client_emc(w_ws_client_emc &&p_other) = default;
//  // move assignment operator.
//  W_API w_ws_client_emc &operator=(w_ws_client_emc &&p_other) = default;
//
//  // destructor
//  W_API virtual ~w_ws_client_emc() noexcept;
//
//  W_API boost::leaf::result<int>
//  init(_In_ EmscriptenWebSocketCreateAttributes &p_create_attribute) noexcept;
//
//  W_API void
//  set_onopen_callback(_In_ em_websocket_open_callback_func p_on_open_cb,
//                      _In_ void *p_user_data = nullptr) noexcept;
//
//  /*
//   * get whether websocket is open or not
//   * @returns true if socket was open
//   */
//  W_API bool is_open() const noexcept;
//
//private:
//  // copy constructor
//  w_ws_client_emc(const w_ws_client_emc &) = delete;
//  // copy operator
//  w_ws_client_emc &operator=(const w_ws_client_emc &) = delete;
//
//  EMSCRIPTEN_WEBSOCKET_T _ws = -1;
//};
//} // namespace wolf::system::socket

#endif //defined(WOLF_SYSTEM_HTTP1_WS) && defined(EMSCRIPTEN)
