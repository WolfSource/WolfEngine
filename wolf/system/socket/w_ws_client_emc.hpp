#if defined(WOLF_SYSTEM_HTTP1_WS)// && defined(EMSCRIPTEN)

#pragma once

#include <wolf/wolf.hpp>
#include <variant>
#include <vector>
#include <cstddef>

namespace wolf::system::socket {

using w_ws_message_variant = std::variant<w_binary, std::string>;

class w_ws_client_emc {
 public:
  W_API w_ws_client_emc() noexcept = default;
  W_API virtual ~w_ws_client_emc() noexcept { std::ignore = close(); }

  // move constructor.
  W_API w_ws_client_emc(w_ws_client_emc &&p_other) noexcept {
    _move(std::forward<w_ws_client_emc &&>(p_other));
  }
  // move assignment operator.
  W_API w_ws_client_emc &operator=(w_ws_client_emc &&p_other) noexcept {
    _move(std::forward<w_ws_client_emc &&>(p_other));
    return *this;
  }

  boost::leaf::result<int> open(_In_ const std::string &p_url,
                                _In_ w_function<void(void) noexcept> &&p_on_open_handler,
                                _In_ w_function<void(void) noexcept> &&p_on_message_handler,
                                _In_ w_function<void(std::string) noexcept> &&p_on_error_handler,
                                _In_ w_function<void(void) noexcept> &&p_on_close_handler) noexcept;

  W_API boost::leaf::result<int> close() noexcept;

  // bool send(_In_ w_ws_message_variant p_data) noexcept;
  // bool send(_In_ const byte *p_data, _In_ size_t p_size) noexcept;

  // bool is_open() const noexcept;
  // bool is_closed() const noexcept;

  // wolf::function<void()> on_open_callback;

 private:
  // copy constructor
  w_ws_client_emc(const w_ws_client_emc &) = delete;
  // copy operator
  w_ws_client_emc &operator=(const w_ws_client_emc &) = delete;

  static void s_on_open_callback(_In_ void *p_ptr) noexcept;
  static void s_on_error_callback(_In_ const char *p_error, _In_ void *p_ptr) noexcept;
  static void s_on_message_callback(_In_ const char *p_data, _In_ int p_size,
                                    _In_ void *p_ptr) noexcept;

  w_function<void(void) noexcept> _on_open_handler;
  // w_function<void(void) noexcept> &&_on_message_handler;
  w_function<void(std::string) noexcept> _on_error_handler;
  // w_function<void(void) noexcept> &&_on_close_handler;

  int _id = 0;
  bool _connected = false;
};
}  // namespace wolf::system::socket

#endif // defined(WOLF_SYSTEM_HTTP1_WS) && defined(EMSCRIPTEN)
