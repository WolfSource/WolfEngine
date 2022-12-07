#ifdef WOLF_STREAM_RIST

#include "w_rist.hpp"

#ifndef __clang__
#include <Objbase.h>
#endif

using w_rist = wolf::stream::rist::w_rist;
using w_rist_data_block = wolf::stream::rist::w_rist_data_block;

#pragma region callbacks

static int s_on_log_callback(_In_ void *p_arg, _In_ rist_log_level p_log_level,
                             _In_z_ const char *p_msg) {
  const gsl::not_null<w_rist *> _rist_nn(gsl::narrow_cast<w_rist *>(p_arg));
  if (_rist_nn->on_log_callback) {
    _rist_nn->on_log_callback(p_log_level, p_msg);
  }
  return S_OK;
}

static int s_on_auth_handler_connect_callback(_In_ void *p_arg,
                                              _In_z_ const char *p_conn_ip,
                                              _In_ uint16_t p_conn_port,
                                              _In_z_ const char *p_local_ip,
                                              _In_ uint16_t p_local_port,
                                              _In_ rist_peer *p_peer) {
  const auto _rist_nn =
      gsl::not_null<w_rist *>(gsl::narrow_cast<w_rist *>(p_arg));

  if (_rist_nn->on_auth_connected_callback) {
    _rist_nn->on_auth_connected_callback(p_conn_ip, p_conn_port, p_local_ip,
                                         p_local_port);
  }

  return S_OK;
}

static int s_on_auth_handler_disconnect_callback(_In_ void *p_arg,
                                                 _In_ rist_peer *p_peer) {
  const auto _rist_nn =
      gsl::not_null<w_rist *>(gsl::narrow_cast<w_rist *>(p_arg));
  if (_rist_nn->on_auth_disconnected_callback) {
    _rist_nn->on_auth_disconnected_callback();
  }
  return S_OK;
}

static int s_on_receiver_data_callback(_In_ void *p_arg,
                                       _In_ rist_data_block *p_data_block) {
  const auto _rist_nn =
      gsl::not_null<w_rist *>(gsl::narrow_cast<w_rist *>(p_arg));
  const auto _data_block_nn = gsl::not_null<rist_data_block *>(p_data_block);

  if (_rist_nn->on_receiver_data_callback) {
    auto _block = w_rist_data_block(std::move(*p_data_block));
    _rist_nn->on_receiver_data_callback(_block);
  } else {
    // release block
    rist_receiver_data_block_free2(&p_data_block);
  }
  return S_OK;
}

#pragma endregion

w_rist::w_rist(_In_ rist_ctx_mode p_mode, _In_ rist_profile p_profile,
               _In_ uint16_t p_loss_percentage,
               _In_ rist_log_level p_log_level) noexcept
    : _mode(p_mode), _profile(p_profile), _loss_percentage(p_loss_percentage),
      _log_level(p_log_level), _log(nullptr), _ctx(nullptr), _peer(nullptr) {}
 
 w_rist::~w_rist() noexcept {
  if (this->_ctx != nullptr) {
    if (this->_peer != nullptr) {
      std::ignore = rist_peer_destroy(this->_ctx, this->_peer);
      this->_peer = nullptr;
    }
    std::ignore = rist_destroy(this->_ctx);
    this->_ctx = nullptr;
  }
  _release_log();
}

 void w_rist::_release_log() {
  if (this->_log == nullptr)
    return;

  rist_logging_unset_global();
  rist_logging_settings_free2(&this->_log);
}

boost::leaf::result<int> w_rist::init() {

  // first release resources of context and log
  _release_log();
  if (this->_ctx != nullptr) {
    std::ignore = rist_destroy(this->_ctx);
  }

  // now create a log
  if (rist_logging_set(&this->_log, this->_log_level, s_on_log_callback, this,
                       nullptr, stderr) != 0) {
    return W_FAILURE(std::errc::operation_canceled,
                 "could not create a rist log callback");
  }

  // create a rist context
  constexpr uint16_t _max_loss = 22;
  if (this->_mode == rist_ctx_mode::RIST_SENDER_MODE) {

    // create a sender
    const auto _ret =
        rist_sender_create(&this->_ctx, this->_profile, 0, this->_log);
    if (_ret != 0) {
      return W_FAILURE(std::errc::operation_canceled,
                   "could not create a rist sender context");
    }

    if (this->_loss_percentage > 0) {
      this->_ctx->sender_ctx->simulate_loss = true;
      this->_ctx->sender_ctx->loss_percentage =
          this->_loss_percentage > _max_loss ? _max_loss
                                             : this->_loss_percentage;
    }
  } else {
    // create a receiver
    const auto _ret =
        rist_receiver_create(&this->_ctx, this->_profile, this->_log);

    if (_ret != 0) {
      return W_FAILURE(std::errc::operation_canceled,
                   "could not create a rist receiver context");
    }

    if (this->_loss_percentage > 0) {
      this->_ctx->receiver_ctx->simulate_loss = true;
      this->_ctx->receiver_ctx->loss_percentage =
          this->_loss_percentage > _max_loss ? _max_loss
                                             : this->_loss_percentage;
    }

    if (rist_receiver_data_callback_set2(
            this->_ctx, s_on_receiver_data_callback, this) != 0) {
      return W_FAILURE(std::errc::operation_canceled,
                   "could not set data receiver callback");
    }
  }

  if (rist_auth_handler_set(this->_ctx, s_on_auth_handler_connect_callback,
                            s_on_auth_handler_disconnect_callback, this) != 0) {
    return W_FAILURE(std::errc::operation_canceled,
                 "could not set rist auth handler");
  }

  return S_OK;
}

boost::leaf::result<int>
w_rist::connect(_In_ const std::string_view p_endpoint) {

  if (p_endpoint.empty()) {
    return W_FAILURE(std::errc::bad_address, "missing endpoint url");
  }

  if (this->_peer != nullptr) {
    std::ignore = rist_peer_destroy(this->_ctx, this->_peer);
  }

  // rely on the library to parse the url
  auto *_peer_config =
      gsl::owner<rist_peer_config *>(calloc(1, sizeof(rist_peer_config)));
  // check for memory violation
  std::ignore = gsl::not_null<rist_peer_config *>(_peer_config);


  rist_peer_config_defaults_set(_peer_config);
  //if (parse_url_options(url_local, _peer_config) != 0) {
  //  return W_FAILURE(std::errc::bad_address,
  //               "could not parse peer options for receiver");
  //}

  if (rist_parse_address2(p_endpoint.data(), &_peer_config) != 0) {
    return W_FAILURE(std::errc::bad_address,
                 "could not parse peer options for receiver");
  }

  if (rist_peer_create(this->_ctx, &this->_peer, _peer_config) != 0) {
    return W_FAILURE(std::errc::operation_canceled,
                 "could not add peer connector to receiver");
  }

  free(_peer_config);

  if (rist_start(this->_ctx) != 0) {
    return W_FAILURE(std::errc::operation_canceled,
                 "could not add peer connector to receiver");
  }

  return S_OK;
}

boost::leaf::result<size_t>
w_rist::send(_In_ const w_rist_data_block &p_block) {
  auto _bytes = rist_sender_data_write(this->_ctx, p_block._block);
  return _bytes >= 0
             ? boost::leaf::result<size_t>(gsl::narrow_cast<size_t>(_bytes))
             : W_FAILURE(std::errc::no_message,
                     "could not send data block to the rist stream");
}

boost::leaf::result<size_t> w_rist::receive(_Inout_ w_rist_data_block &p_block,
                                            _In_ int p_timeout_ms) {
  const gsl::not_null<rist_ctx *> _ctx_nn(this->_ctx);
  auto _bytes =
      rist_receiver_data_read2(_ctx_nn, &p_block._block, p_timeout_ms);
  return _bytes >= 0
             ? boost::leaf::result<size_t>(gsl::narrow_cast<size_t>(_bytes))
             : W_FAILURE(std::errc::no_message,
                     "could not read data block from the rist stream");
  return S_OK;
}

#endif
