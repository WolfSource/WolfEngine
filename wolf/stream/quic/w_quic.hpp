#pragma once

#ifdef WOLF_STREAM_QUIC

#include <wolf.hpp>
#include <boost/signals2.hpp>

namespace wolf::stream::quic
{
    enum w_quic_global_flags
    {
        W_QUIC_GLOBAL_CLIENT = (1 << 0),
        W_QUIC_GLOBAL_SERVER = (1 << 1),
    };

    enum w_quic_flags
    {
        W_QUIC_CLIENT = 0,
        W_QUIC_SERVER = (1 << 0),
        W_QUIC_ENABLE_HTTP3 = (1 << 1),
    };

    class w_quic
    {
    public:
        //struct service_port;
        w_quic(w_quic_flags p_server_type);
        virtual ~w_quic();

        W_RESULT init();

        boost::signals2::signal<void()> on_connection_new_sig;
        boost::signals2::signal<void()> on_connection_closed_sig;
        
        static W_RESULT init_global(w_quic_global_flags p_global_flags);
        static W_RESULT fini_global();

    private:
        struct ctx;
        std::unique_ptr<ctx> _ctx;
    };
} // namespace wolf::stream::quic

#endif


