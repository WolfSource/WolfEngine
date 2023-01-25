/*
    Project: Wolf Engine. Copyright � 2014-2023 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#ifdef WOLF_STREAM_RIST

#pragma once

#include "w_rist_data_block.hpp"
#include <wolf/wolf.hpp>

namespace wolf::stream::rist {
class w_rist {
public:
  /*
   * constructor of the rist stream
   * @param p_mode, the context mode
   * @param p_profile, the rist profile
   * @param p_loss_percentage, the loss percentage of rist stream
   * @param p_log_level, the log level
   */
  W_API w_rist(_In_ rist_ctx_mode p_mode, _In_ rist_profile p_profile,
               _In_ uint16_t p_loss_percentage,
               _In_ rist_log_level p_log_level) noexcept;

  // move constructor.
  W_API w_rist(w_rist &&p_other) noexcept = default;
  // move assignment operator.
  W_API w_rist &operator=(w_rist &&p_other) noexcept = default;

  // destructor
  W_API virtual ~w_rist();

  /*
   * initialize ris stream
   * @returns result
   */
  W_API boost::leaf::result<int> init();

  /**
   * connect or bind to the url
   * @param p_endpoint, the destination endpoint of rist stream
   */
  W_API
  boost::leaf::result<int> connect(_In_ std::string_view p_endpoint);

  /**
   * write a data block to the rist stream
   * @param p_block, the data block
   * @returns the number of written bytes with result
   */
  W_API boost::leaf::result<size_t> send(_In_ const w_rist_data_block &p_block);

  ///**
  // * set data to rist data block
  // * @param p_block, the data block
  // * @param p_timeout, read timeout in milliseconds
  // * @returns the number of read bytes with result
  // */
  // W_API boost::leaf::result<size_t> receive(_Inout_ w_rist_data_block
  // &p_block,
  //                                          _In_ int p_timeout_ms);

  // this callback will be called on log
  std::function<void(_In_ rist_log_level /*p_log_level*/,
                     _In_ std::string_view /*p_msg*/)>
      on_log_callback;

  // this signal will be called on auth connection
  std::function<void(
      _In_z_ std::string_view /*p_conn_ip*/, _In_ uint16_t /*p_conn_port*/,
      _In_z_ std::string_view /*p_local_ip*/, _In_ uint16_t /*p_local_port*/)>
      on_auth_connected_callback;

  // this signal will be called after disconnect
  std::function<void()> on_auth_disconnected_callback;

  // this signal will be called on receiving data
  std::function<void(_In_ const w_rist_data_block &)> on_receiver_data_callback;

private:
  w_rist(const w_rist &) = delete;
  w_rist &operator=(const w_rist &) = delete;
  void _release_log();

  rist_ctx_mode _mode = rist_ctx_mode::RIST_SENDER_MODE;
  rist_profile _profile = rist_profile::RIST_PROFILE_MAIN;
  uint16_t _loss_percentage = 0;
  rist_log_level _log_level = rist_log_level::RIST_LOG_DEBUG;
  rist_logging_settings *_log = nullptr;
  rist_ctx *_ctx = nullptr;
  rist_peer *_peer = nullptr;
};
} // namespace wolf::stream::rist

#endif // WOLF_STREAM_RIST
