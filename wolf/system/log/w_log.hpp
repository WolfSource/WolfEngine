///*
//    Project: Wolf Engine. Copyright © 2014-2022 Pooya Eimandar
//    https://github.com/WolfEngine/WolfEngine
//*/
//
//#pragma once
//
//#ifdef WOLF_SYSTEM_LOG
//
//#include <wolf.hpp>
//#include "w_log_config.hpp"
//
//namespace wolf::system::log
//{
//    class w_log
//    {
//    public:
//        // constructor
//        W_API explicit w_log(_In_ w_log_config p_log_config) noexcept;
//
//        // disable copy constructor
//        w_log(const w_log&) = delete;
//        // disable copy operator
//        w_log& operator=(const w_log&) = delete;
//
//        // Move constructor.
//        W_API w_log(w_log&& p_other) = delete;
//        // Move assignment operator.
//        W_API w_log& operator=(w_log&& p_other) = delete;
//
//        // destructor
//        W_API virtual ~w_log();
//
//        W_API tl::expected<void, W_RESULT> init();
//
//        template<typename... Args>
//        W_API tl::expected<void, W_RESULT> write(
//            _In_z_ std::string_view p_fmt,
//            _In_ Args &&... p_args);
//
//        template<typename... Args>
//        W_API tl::expected<void, W_RESULT> write(
//            _In_ const w_log_level p_log_level,
//            _In_z_ std::string_view p_fmt,
//            _In_ Args &&... args);
//
//        W_API tl::expected<void, W_RESULT> flush();
//
//        W_API w_log_config get_config() const;
//
//    private:
//        struct priv;
//        std::unique_ptr<priv> _priv;
//
//        w_log_config _config;
//    };
//} // namespace wolf::system::log
//
//#endif