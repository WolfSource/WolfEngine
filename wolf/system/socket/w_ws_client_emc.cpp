#if defined(WOLF_SYSTEM_HTTP1_WS) && defined(EMSCRIPTEN)

#include "w_ws_client_emc.hpp"

using w_ws_client_emc = wolf::system::socket::w_ws_client_emc;

static std::string s_emc_result_to_string(_In_ EMSCRIPTEN_RESULT p_emc_result) {
  switch (p_emc_result) {
  default:
    return "undefined EMSCRIPTEN_RESULT";
  case EMSCRIPTEN_RESULT_SUCCESS:
    return "EMSCRIPTEN_RESULT_SUCCESS";
  case EMSCRIPTEN_RESULT_DEFERRED:
    return "EMSCRIPTEN_RESULT_DEFERRED";
  case EMSCRIPTEN_RESULT_NOT_SUPPORTED:
    return "EMSCRIPTEN_RESULT_NOT_SUPPORTED";
  case EMSCRIPTEN_RESULT_FAILED_NOT_DEFERRED:
    return "EMSCRIPTEN_RESULT_FAILED_NOT_DEFERRED";
  case EMSCRIPTEN_RESULT_INVALID_TARGET:
    return "EMSCRIPTEN_RESULT_INVALID_TARGET";
  case EMSCRIPTEN_RESULT_UNKNOWN_TARGET:
    return "EMSCRIPTEN_RESULT_UNKNOWN_TARGET";
  case EMSCRIPTEN_RESULT_INVALID_PARAM:
    return "EMSCRIPTEN_RESULT_INVALID_PARAM";
  case EMSCRIPTEN_RESULT_FAILED:
    return "EMSCRIPTEN_RESULT_FAILED";
  case EMSCRIPTEN_RESULT_NO_DATA:
    return "EMSCRIPTEN_RESULT_NO_DATA";
  case EMSCRIPTEN_RESULT_TIMED_OUT:
    return "EMSCRIPTEN_RESULT_TIMED_OUT";
  }
}

boost::leaf::result<int> w_ws_client_emc::init(
    _In_ EmscriptenWebSocketCreateAttributes &p_create_attribute) noexcept {
  if (emscripten_websocket_is_supported() < 1) {
    return W_FAILURE(std::errc::not_supported,
                     "emscripten websocket is not supported");
  }

  this->_ws = emscripten_websocket_new(&p_create_attribute);
  if (this->_ws == 0) {
    return W_FAILURE(std::errc::operation_canceled,
                     "could not create emscripten websocket");
  }
  if (this->_ws < 0) {
    return W_FAILURE(std::errc::operation_canceled,
                     "could not create emscripten websocket because: " +
                         s_emc_result_to_string(
                             gsl::narrow_cast<EMSCRIPTEN_RESULT>(this->_ws)));
  }

  return 0;
}

w_ws_client_emc::~w_ws_client_emc() noexcept {
  if (this->_ws > 0) {
    close();
    emscripten_websocket_delete(this->_ws);
    this->_ws = -1;
  }
}

void w_ws_client_emc::set_onopen_callback(_In_ em_websocket_open_callback_func
                                              p_callback,
                                          _In_ void *p_user_data) noexcept {
  this->_is_open = true;
  emscripten_websocket_set_onopen_callback(this->_ws, p_user_data, p_callback);
}

void w_ws_client_emc::set_onerror_callback(_In_ em_websocket_error_callback_func
                                               p_callback,
                                           _In_ void *p_user_data) noexcept {
  emscripten_websocket_set_onerror_callback(this->_ws, p_user_data, p_callback);
}

void w_ws_client_emc::set_onmessage_callback(
    _In_ em_websocket_message_callback_func p_callback,
    _In_ void *p_user_data) noexcept {
  emscripten_websocket_set_onmessage_callback(this->_ws, p_user_data,
                                              p_callback);
}

void w_ws_client_emc::set_onclose_callback(_In_ em_websocket_close_callback_func
                                               p_callback,
                                           _In_ void *p_user_data) noexcept {
  this->_is_open = false;
  emscripten_websocket_set_onclose_callback(this->_ws, p_user_data, p_callback);
}

void w_ws_client_emc::close(_In_ uint16_t p_code,
                            _In_ const std::string &p_reason) noexcept {
  emscripten_websocket_close(this->_ws, p_code, p_reason.data());
}

bool w_ws_client_emc::is_open() const noexcept { return this->_is_open; }

#endif // defined(WOLF_SYSTEM_HTTP1_WS) && defined(EMSCRIPTEN)
