#if defined(WOLF_SYSTEM_HTTP1_WS)// && defined(EMSCRIPTEN)

#include "w_ws_client_emc.hpp"
//#include <emscripten/emscripten.h>

extern "C" {
extern int wsCreateWebSocket(const char *url);
extern void wsDeleteWebSocket(int ws);
extern void wsSetOpenCallback(int ws, void (*openCallback)(void *));
extern void wsSetErrorCallback(int ws, void (*errorCallback)(const char *, void *));
extern void wsSetMessageCallback(int ws, void (*messageCallback)(const char *, int, void *));
extern int wsSendMessage(int ws, const char *buffer, int size);
extern void wsSetUserPointer(int ws, void *ptr);
}

using w_ws_client_emc = wolf::system::socket::w_ws_client_emc;

void w_ws_client_emc::s_on_open_callback(_In_ void *p_ptr) noexcept {
  const auto _ws = gsl::narrow_cast<w_ws_client_emc *>(p_ptr);
  if (_ws) {
    _ws->_connected = true;
    _ws->_on_open_handler();
  }
}

void w_ws_client_emc::s_on_error_callback(_In_ const char *p_error, _In_ void *p_ptr) noexcept {
  const auto _ws = gsl::narrow_cast<w_ws_client_emc *>(p_ptr) ;
  if (_ws) {
    auto _error_msg = std::string(p_error ? p_error : "unknown");
    _ws->_on_error_handler(std::move(_error_msg));
  }
}

void w_ws_client_emc::s_on_message_callback(_In_ const char *p_data, _In_ int p_size,
                                            _In_ void *p_ptr) noexcept {
  const auto _ws = gsl::narrow_cast<w_ws_client_emc *>(p_ptr);
  if (_ws == nullptr) {
    return;
  }

  if (p_data) {
    if (p_size >= 0) {
      auto _bytes = reinterpret_cast<const std::byte *>(p_data);
      _ws->triggerMessage(w_binary(_bytes, _bytes + p_size));
    } else {
      _ws->triggerMessage(std::string(p_data));
    }
  } else {
    _ws->_on_close_handler();
    _ws->close();
  }
}

boost::leaf::result<int> w_ws_client_emc::open(
    _In_ const std::string &p_url, _In_ wolf::w_function<void(void) noexcept> &&p_on_open_handler,
    _In_ wolf::w_function<void(void) noexcept> &&p_on_message_handler,
    _In_ wolf::w_function<void(std::string) noexcept> &&p_on_error_handler,
    _In_ wolf::w_function<void(void) noexcept> &&p_on_close_handler) noexcept {
  if (p_url.empty()) {
    return W_FAILURE(std::errc::invalid_argument, "missing websocket url endpoint");
  }

  // close current websocket
  close();

  this->_on_open_handler = std::move(p_on_open_handler);
  // this->_on_message_handler = std::move(p_on_message_handler);
  this->_on_error_handler = std::move(p_on_error_handler);
  // this->_on_close_handler = std::move(p_on_close_handler);

  this->_id = wsCreateWebSocket(p_url.c_str());
  if (this->_id <= 0) {
    return W_FAILURE(std::errc::operation_canceled, "websocket is not supported");
  }

  wsSetUserPointer(this->_id, this);
  wsSetOpenCallback(this->_id, s_on_open_callback);
  wsSetErrorCallback(this->_id, s_on_error_callback);
  wsSetMessageCallback(this->_id, s_on_message_callback);

  return 0;
}

#endif // defined(WOLF_SYSTEM_HTTP1_WS) && defined(EMSCRIPTEN)
