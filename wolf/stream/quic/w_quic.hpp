///*
//    Project: Wolf Engine. Copyright © 2014-2022 Pooya Eimandar
//    https://github.com/WolfEngine/WolfEngine
//*/
//
//#pragma once
//
//#ifdef WOLF_STREAM_QUIC
//
//#include <wolf.hpp>
//#include <boost/signals2.hpp>
//
//namespace wolf::stream::quic
//{
//    enum class w_quic_type
//    {
//        CLIENT,
//        SERVER,
//    };
//
//    enum class w_quic_profile
//    {
//        LOW_LATENCY, 
//        MAX_THROUGHPUT,
//        SCAVENGER,
//        REAL_TIME,
//    };
//
//    class w_quic
//    {
//    public:
//        //struct service_port;
//        w_quic(
//            std::string p_app_name, 
//            uint16_t p_port, 
//            w_quic_type p_quic_type, 
//            w_quic_profile p_profile);
//        virtual ~w_quic();
//
//        tl::expected<void, std::string> init();
//
//        //boost::signals2::signal<void()> on_connection_new_sig;
//        //boost::signals2::signal<void()> on_connection_closed_sig;
//
//        struct q_ctx;
//        std::unique_ptr<q_ctx> ctx;
//    private:
//        w_quic_type _quic_type;
//        uint16_t _port;
//
//        tl::expected<void, std::string> init_client();
//        tl::expected<void, std::string> init_server();
//        void _release()  noexcept;
//    };
//} // namespace wolf::stream::quic
//
//#endif
//
//
