//#ifdef WOLF_STREAM_RIST
//
//#include "w_rist.hpp"
//#include <librist/librist.h>
//#include <rist-private.h>
//
//using w_rist = wolf::stream::w_rist;
//
//struct w_rist::priv
//{
//    priv() = default;
//
//    priv(const priv&) = delete;
//    priv(priv&&) = delete;
//    priv& operator=(const priv&) = delete;
//    priv& operator=(priv&&) = delete;
//
//    ~priv()
//    {
//        if (this->ctx)
//        {
//            rist_destroy(this->ctx);
//        }
//        if (this->log) 
//        {
//            free(this->log);
//        }
//    }
//
//	rist_ctx* ctx = nullptr;
//	rist_peer* peer = nullptr;
//	rist_profile profile;
//	uint16_t loss_percentage = 10;
//	rist_logging_settings* log = nullptr;
//};
//
//// callbacks
//int s_on_log_callback(void* p_arg, rist_log_level p_log_level, const char* p_msg)
//{
//    auto _rist = gsl::narrow_cast<w_rist*>(p_arg);
//    if (_rist != nullptr)
//    {
//        using log_level = wolf::stream::w_rist::log_level;
//        _rist->on_log_sig(gsl::narrow_cast<log_level>(p_log_level), p_msg);
//    }
//
//    return 0;
//}
//
//int s_on_auth_handler_connect_callback(
//    void* p_arg,
//    const char* p_conn_ip,
//    uint16_t p_conn_port,
//    const char* p_local_ip,
//    uint16_t p_local_port,
//    rist_peer* p_peer)
//{
//    auto _rist = gsl::narrow_cast<w_rist*>(p_arg);
//    if (_rist != nullptr)
//    {
//        _rist->on_auth_connected_sig(
//            p_conn_ip,
//            p_conn_port,
//            p_local_ip,
//            p_local_port);
//    }
//    return 0;
//}
//
//int s_on_auth_handler_disconnect_callback(void* p_arg, rist_peer* p_peer)
//{
//    auto _rist = gsl::narrow_cast<w_rist*>(p_arg);
//    if (_rist != nullptr)
//    {
//        _rist->on_auth_disconnected_sig();
//    }
//    return 0;
//}
//
//int s_on_receiver_data_callback(void* p_arg, rist_data_block* p_data_block)
//{
//    auto _rist = gsl::narrow_cast<w_rist*>(p_arg);
//    if (_rist != nullptr && p_data_block != nullptr)
//    {
//        _rist->on_receiver_data_sig(p_data_block->payload, p_data_block->payload_len);
//    }
//    return 0;
//}
//
//w_rist::w_rist() : _priv(std::make_unique<w_rist::priv>())
//{
//}
//
//w_rist::~w_rist()
//{
//}
//
//tl::expected<void, w_result> w_rist::init(
//    _In_ mode p_mode,
//    _In_ profile p_profile,
//    _In_ uint16_t p_loss_percentage,
//    _In_ log_level p_log_level)
//{
//    this->_priv->profile = gsl::narrow_cast<rist_profile>(p_profile);
//    this->_priv->loss_percentage = p_loss_percentage;
//
//    // create rist context
//    auto _log_level = gsl::narrow_cast<rist_log_level>(p_log_level);
//    switch (p_mode)
//    {
//    case mode::SENDER:
//    {
//        // create a log
//        if (rist_logging_set(
//            &this->_priv->log,
//            _log_level,
//            s_on_log_callback,
//            this,
//            nullptr,
//            stderr) != 0)
//        {
//            auto _res = w_result("could not create rist log for sender");
//            return tl::unexpected(_res);
//        }
//        // create a sender
//        auto ret = rist_sender_create(&this->_priv->ctx, this->_priv->profile, 0, this->_priv->log);
//        if (ret != 0)
//        {
//            auto _res = w_result("could not create rist sender context");
//            return tl::unexpected(_res);
//        }
//        if (p_loss_percentage > 0)
//        {
//            this->_priv->ctx->sender_ctx->simulate_loss = true;
//            this->_priv->ctx->sender_ctx->loss_percentage = p_loss_percentage * 10;
//        }
//        break;
//    }
//    case mode::RECEIVER:
//    {
//        //create a log
//        if (rist_logging_set(
//            &this->_priv->log,
//            _log_level,
//            s_on_log_callback,
//            this,
//            nullptr,
//            stderr) != 0)
//        {
//            auto _res = w_result("could not create rist log for receiver");
//            return tl::unexpected(_res);
//        }
//        // create a receiver
//        auto ret = rist_receiver_create(&this->_priv->ctx, this->_priv->profile, this->_priv->log);
//        if (ret != 0)
//        {
//            auto _res = w_result("could not create rist receiver context");
//            return tl::unexpected(_res);
//        }
//        if (p_loss_percentage > 0)
//        {
//            this->_priv->ctx->receiver_ctx->simulate_loss = true;
//            this->_priv->ctx->receiver_ctx->loss_percentage = p_loss_percentage * 10;
//        }
//
//        break;
//    }
//    };
//
//    if (rist_auth_handler_set(
//        this->_priv->ctx,
//        s_on_auth_handler_connect_callback,
//        s_on_auth_handler_disconnect_callback,
//        this) != 0)
//    {
//        auto _res = w_result("could not set rist auth handler");
//        return tl::unexpected(_res);
//    }
//
//    if (rist_receiver_data_callback_set2(
//        this->_priv->ctx,
//        s_on_receiver_data_callback,
//        this) != 0)
//    {
//        auto _res = w_result("could not set data_callback");
//        return tl::unexpected(_res);
//    }
//
//    W_EXPECTED_VOID
//}
//
//tl::expected<void, w_result> w_rist::connect(_In_ std::string_view p_url)
//{
//    if (p_url.empty())
//    {
//        auto _res = w_result("missing url");
//        return tl::unexpected(_res);
//    }
//
//    // rely on the library to parse the url
//    rist_peer_config* peer_config = nullptr;
//    if (rist_parse_address2(p_url.data(), &peer_config))
//    {
//        auto _res = w_result("could not parse peer options for receiver");
//        return tl::unexpected(_res);
//    }
//
//    if (rist_peer_create(
//        this->_priv->ctx, 
//        &this->_priv->peer, 
//        peer_config) == -1) 
//    {
//        auto _res = w_result("could not add peer connector to receiver");
//        return tl::unexpected(_res);
//    }
//
//    free((void*)peer_config);
//
//    if (rist_start(this->_priv->ctx) == -1)
//    {
//        auto _res = w_result("could not add peer connector to receiver");
//        return tl::unexpected(_res);
//    }
//
//    W_EXPECTED_VOID
//}
//
//tl::expected<int, w_result> w_rist::send(_Inout_ w_rist_data_block& p_block)
//{
//    auto _ret = p_block.get_internal_block()
//        .map([&](tl::expected<rist_data_block*, w_result> p_data)
//            {
//                auto _v = p_data.value();
//                return rist_sender_data_write(this->_priv->ctx, _v);
//            }).map_error([](w_result p_error)
//                {
//                    return p_error;
//                });
//
//            return _ret;
//}
//
//
//tl::expected<int, w_result> w_rist::receive(_Inout_ w_rist_data_block& p_block, _In_ int p_timeout)
//{
//    auto _ret = p_block.get_internal_block()
//        .map([&](tl::expected<rist_data_block*, w_result> p_data)
//            {
//                auto _v = p_data.value();
//                return rist_receiver_data_read2(this->_priv->ctx, &_v, p_timeout);
//            }).map_error([](w_result p_error)
//                {
//                    return p_error;
//                });
//
//            return _ret;
//}
//
//#endif