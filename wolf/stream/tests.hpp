/*
    Project: Wolf Engine. Copyright © 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#pragma once

#ifdef WOLF_STREAM_RIST

#include <wolf.hpp>
#include "w_rist_data_block.hpp"
#include <boost/signals2.hpp>

namespace wolf::stream
{
    class w_rist
    {
    public:
        enum class profile
        {
            SIMPLE = 0,
            MAIN,
            ADVANCED,
        };
        enum class log_level
        {
            DISABLE = -1,
            ERROR_ = 3,
            WARN = 4,
            NOTICE = 5,
            INFO = 6,
            DEBUG_ = 7,
            SIMULATE = 100,
        };
        enum class mode
        {
            SENDER,
            RECEIVER
        };

        w_rist();

        w_rist(const w_rist&) = delete;
        w_rist(w_rist&&) = delete;
        w_rist& operator=(const w_rist&) = delete;
        w_rist& operator=(w_rist&&) = delete;

        virtual ~w_rist();

        // this signal will be called on log
        boost::signals2::signal<void(log_level/*p_log_level*/, std::string_view /*p_msg*/)> on_log_sig;
        
        // this signal will be called on auth connection
        boost::signals2::signal<void(
            std::string_view /*p_conn_ip*/, 
            uint16_t /*p_conn_port*/, 
            std::string_view /*p_local_ip*/, 
            uint16_t /*p_local_port*/)> on_auth_connected_sig;

        // this signal will be called after disconnect
        boost::signals2::signal<void()> on_auth_disconnected_sig;

        // this signal will be called on receiving data
        boost::signals2::signal<void(const void*, size_t)> on_receiver_data_sig;

       /**
        * initialize rist stream
        * @param p_mode, the context mode
        * @param p_profile, the rist profile
        * @param p_loss_percentage, the loss percentage of rist stream
        * @param p_log_level, the log level
        * @returns an expected with void data or an unexpected with error message
        */
        W_API tl::expected<void, w_result> init(
            _In_ mode p_mode,
            _In_ profile p_profile,
            _In_ uint16_t p_loss_percentage,
            _In_ log_level p_log_level);
        
       /**
        * connect or bind
        * @param p_url, data of rist
        * @returns void
        */
        W_API tl::expected<void, w_result> connect(_In_ std::string_view p_url);

       /**
        * write a data block to the rist stream
        * @param p_block, the data block
        * @returns the number of written bytes or an unexpected with error message
        */
        W_API tl::expected<int, w_result> send(_Inout_ w_rist_data_block& p_block);

       /**
        * set data to rist data block
        * @param p_block, the data block
        * @param p_timeout, read timeout
        * @returns the number of read bytes or an unexpected with error message
        */
        W_API tl::expected<int, w_result> receive(_Inout_ w_rist_data_block& p_block, _In_ int p_timeout);

    private:
        struct priv;
        std::unique_ptr<priv> _priv;
    };
} // namespace wolf::stream::rist

#endif


