//#ifdef WOLF_STREAM_QUIC
//
//#include "w_quic.hpp"
//
//#ifdef WIN32
//#pragma warning(disable:5105)
//#endif
//
//#include "msquic.h"
//
//using w_quic = wolf::stream::quic::w_quic;
//using w_quic_profile = wolf::stream::quic::w_quic_profile;
//
//struct QUIC_CREDENTIAL_CONFIG_HELPER {
//    QUIC_CREDENTIAL_CONFIG CredConfig;
//    union {
//        QUIC_CERTIFICATE_HASH CertHash;
//        QUIC_CERTIFICATE_HASH_STORE CertHashStore;
//        QUIC_CERTIFICATE_FILE CertFile;
//        QUIC_CERTIFICATE_FILE_PROTECTED CertFileProtected;
//    };
//};
//
//// The protocol name used in the Application Layer Protocol Negotiation (ALPN).
//const QUIC_BUFFER S_ALPN = { sizeof("sample") - 1, (uint8_t*)"sample" };
//
//// forward declrations for callbacks
//
//// The server's callback for stream events from MsQuic.
//_IRQL_requires_max_(DISPATCH_LEVEL)
//_Function_class_(QUIC_STREAM_CALLBACK)
//QUIC_STATUS QUIC_API s_server_stream_callback(
//    _In_ HQUIC p_stream,
//    _In_opt_ void* p_context,
//    _Inout_ QUIC_STREAM_EVENT* p_event);
//
//// The server's callback for connection events from MsQuic.
//_IRQL_requires_max_(DISPATCH_LEVEL)
//_Function_class_(QUIC_CONNECTION_CALLBACK)
//QUIC_STATUS QUIC_API s_server_connection_callback(
//    _In_ HQUIC p_connection,
//    _In_opt_ void* p_context,
//    _Inout_ QUIC_CONNECTION_EVENT* p_event) noexcept;
//
//
//// The server's callback for listener events from MsQuic.
//_IRQL_requires_max_(PASSIVE_LEVEL)
//_Function_class_(QUIC_LISTENER_CALLBACK)
//QUIC_STATUS QUIC_API s_server_listener_callback(
//    _In_ HQUIC p_listener,
//    _In_opt_ void* p_context,
//    _Inout_ QUIC_LISTENER_EVENT* p_event);
//
//struct w_quic::q_ctx
//{
//    const QUIC_API_TABLE* api = nullptr;
//    QUIC_REGISTRATION_CONFIG reg_config = {};
//    HQUIC handle = nullptr;
//    HQUIC listener = nullptr;
//    HQUIC config = nullptr;
//
//    w_quic::q_ctx(std::string p_app_name, w_quic_profile p_profile) noexcept
//    {
//        this->reg_config =
//        {
//            p_app_name.c_str(),
//            gsl::narrow_cast<QUIC_EXECUTION_PROFILE>(p_profile)
//        };
//    }
//};
//
//w_quic::w_quic(
//    std::string p_app_name,
//    uint16_t p_port,
//    w_quic_type p_quic_type,
//    w_quic_profile p_profile) :
//    _port(p_port),
//    _quic_type(p_quic_type),
//    ctx(std::make_unique<w_quic::q_ctx>(p_app_name, p_profile))
//{
//}
//
//w_quic::~w_quic()
//{
//    _release();
//}
//
//tl::expected<void, std::string> w_quic::init()
//{
//    constexpr auto TRACE = "w_quic::init";
//
//    if (this->ctx == nullptr)
//    {
//        auto _msg = fmt::format("bad context. trace: {} \n", TRACE);
//        return tl::unexpected(_msg);
//    }
//
//    // Open a handle to the library and get the API function table.
//    auto _ret = MsQuicOpen2(&this->ctx->api);
//    if (_ret != S_OK)
//    {
//        auto _msg = fmt::format("MsQuicOpen2 failed, {:x}. trace: {} \n", _ret, TRACE);
//        return tl::unexpected(_msg);
//    }
//
//    // Create a registration for the app's connections.
//    _ret = this->ctx->api->RegistrationOpen(&this->ctx->reg_config, &this->ctx->handle);
//    if (_ret != S_OK)
//    {
//        auto _msg = fmt::format("RegistrationOpen failed, {:x}. trace: {} \n", _ret, TRACE);
//        return tl::unexpected(_msg);
//    }
//
//    switch (this->_quic_type)
//    {
//    default:
//    {
//        auto _msg = fmt::format("Undefined quic server type. trace: {} \n", TRACE);
//        return tl::unexpected(_msg);
//    }
//    case w_quic_type::CLIENT:
//    {
//        return init_client();
//    }
//    case w_quic_type::SERVER:
//    {
//        return init_client();
//    }
//    }
//}
//
//tl::expected<void, std::string> w_quic::init_client()
//{
//    return tl::expected<void, std::string>();
//}
//
//tl::expected<void, std::string> w_quic::init_server()
//{
//    constexpr auto TRACE = "w_quic::init_server";
//
//    QUIC_STATUS _ret;
//
//    // configures the address used for the listener to listen on all IP addresses and the given UDP port
//    QUIC_ADDR _address = { 0 };
//    QuicAddrSetFamily(&_address, QUIC_ADDRESS_FAMILY_UNSPEC);
//    QuicAddrSetPort(&_address, this->_port);
//
//    // Load the server configuration based on the command line.
//    //if (!ServerLoadConfiguration(argc, argv))
//    //{
//    //    return;
//    //}
//
//    // create/allocate a new listener object.
//    //auto _hr = this->ctx->api->ListenerOpen(
//    //    this->ctx->handle,
//    //    s_server_listener_callback,
//    //    gsl::narrow_cast<void*>(this->ctx.get()),
//    //    &this->ctx->listener);
//    //if (_hr != S_OK)
//    //{
//    //    auto _msg = fmt::format("ListenerOpen failed, error code:{:x}. trace: {}\n", _ret, TRACE);
//    //    return tl::unexpected(_msg);
//    //}
//
//    // Starts listening for incoming connections.
//    _ret =  this->ctx->api->ListenerStart(
//        this->ctx->listener, 
//        &S_ALPN,
//        1, 
//        &_address);
//    if (_ret != S_OK)
//    {
//        auto _msg = fmt::format("Undefined quic server type. trace: {} \n", TRACE);
//        return tl::unexpected(_msg);
//
//        //printf("ListenerStart failed, 0x%x!\n", Status);
//    }
//
//    return tl::expected<void, std::string>();
//}
//
//void w_quic::_release() noexcept
//{
//    if (this->ctx != nullptr &&
//        this->ctx->api != nullptr &&
//        this->ctx->listener != nullptr)
//    {
//        this->ctx->api->ListenerClose(this->ctx->listener);
//    }
//}
//
//// The server's callback for stream events from MsQuic.
//_IRQL_requires_max_(DISPATCH_LEVEL)
//_Function_class_(QUIC_STREAM_CALLBACK)
//QUIC_STATUS QUIC_API s_server_stream_callback(
//    _In_ HQUIC p_stream,
//    _In_opt_ void* p_context,
//    _Inout_ QUIC_STREAM_EVENT* p_event)
//{
//    auto _ctx = gsl::narrow_cast<w_quic::q_ctx*>(p_context);
//    if (_ctx == nullptr)
//    {
//        return QUIC_STATUS_INVALID_PARAMETER;
//    }
//
//    switch (p_event->Type) 
//    {
//    case QUIC_STREAM_EVENT_SEND_COMPLETE:
//    {
//        // A previous StreamSend call has completed, and the context is being
//        // returned back to the app.
//        free(p_event->SEND_COMPLETE.ClientContext);
//        //fmt::print("stream {} data sent\n", p_stream);
//        break;
//    }
//    case QUIC_STREAM_EVENT_RECEIVE:
//    {
//        // Data was received from the peer on the stream.
//        //fmt::print("stream {} data received\n", p_stream);
//        break;
//    }
//    case QUIC_STREAM_EVENT_PEER_SEND_SHUTDOWN:
//    {
//        // The peer gracefully shut down its send direction of the stream.
//        //printf("[strm][%p] Peer shut down\n", Stream);
//        //ServerSend(Stream);
//        break;
//    }
//    case QUIC_STREAM_EVENT_PEER_SEND_ABORTED:
//    {
//        //
//        // The peer aborted its send direction of the stream.
//        //
//        //printf("[strm][%p] Peer aborted\n", Stream);
//        _ctx->api->StreamShutdown(p_stream, QUIC_STREAM_SHUTDOWN_FLAG_ABORT, 0);
//        break;
//    }
//    case QUIC_STREAM_EVENT_SHUTDOWN_COMPLETE:
//    {
//        //
//        // Both directions of the stream have been shut down and MsQuic is done
//        // with the stream. It can now be safely cleaned up.
//        //
//        //printf("[strm][%p] All done\n", Stream);
//        _ctx->api->StreamClose(p_stream);
//        break;
//    }
//    default:
//        break;
//    }
//    return QUIC_STATUS_SUCCESS;
//}
//
//// Helper function to load a server configuration. Uses the command line
//// arguments to load the credential part of the configuration.
//W_RESULT s_server_load_configuration(w_quic::q_ctx* p_ctx)
//{
//    if (p_ctx == nullptr)
//    {
//        return W_RESULT::FAILURE;
//    }
//
//    // the following setting should be provided from inputs
//    constexpr uint64_t _idle_timeout_ms = 1000;
//
//    QUIC_SETTINGS settings = { 0 };
//    // Configures the server's idle timeout.
//    settings.IdleTimeoutMs = _idle_timeout_ms;
//    settings.IsSet.IdleTimeoutMs = 1;
//    // Configures the server's resumption level to allow for resumption and 0-RTT.
//    settings.ServerResumptionLevel = QUIC_SERVER_RESUME_AND_ZERORTT;
//    settings.IsSet.ServerResumptionLevel = 1;
//    // Configures the server's settings to allow for the peer to open a single
//    // bidirectional stream. By default connections are not configured to allow
//    // any streams from the peer.
//    settings.PeerBidiStreamCount = 1;
//    settings.IsSet.PeerBidiStreamCount = 1;
//
//    QUIC_CREDENTIAL_CONFIG_HELPER _config;
//    memset(&_config, 0, sizeof(_config));
//    _config.CredConfig.Flags = QUIC_CREDENTIAL_FLAG_NONE;
//
//    const char* _cert = nullptr;
//    const char* _key_file = nullptr;
//    //if ((Cert = GetValue(argc, argv, "cert_hash")) != NULL) {
//    //    //
//    //    // Load the server's certificate from the default certificate store,
//    //    // using the provided certificate hash.
//    //    //
//    //    uint32_t CertHashLen =
//    //        DecodeHexBuffer(
//    //            Cert,
//    //            sizeof(Config.CertHash.ShaHash),
//    //            Config.CertHash.ShaHash);
//    //    if (CertHashLen != sizeof(Config.CertHash.ShaHash)) {
//    //        return FALSE;
//    //    }
//    //    Config.CredConfig.Type = QUIC_CREDENTIAL_TYPE_CERTIFICATE_HASH;
//    //    Config.CredConfig.CertificateHash = &Config.CertHash;
//
//    //}
//    //else if ((Cert = GetValue(argc, argv, "cert_file")) != NULL &&
//    //    (KeyFile = GetValue(argc, argv, "key_file")) != NULL) {
//    //    //
//    //    // Loads the server's certificate from the file.
//    //    //
//    //    const char* Password = GetValue(argc, argv, "password");
//    //    if (Password != nullptr) 
//    //    {
//    //        Config.CertFileProtected.CertificateFile = (char*)Cert;
//    //        Config.CertFileProtected.PrivateKeyFile = (char*)KeyFile;
//    //        Config.CertFileProtected.PrivateKeyPassword = (char*)Password;
//    //        Config.CredConfig.Type = QUIC_CREDENTIAL_TYPE_CERTIFICATE_FILE_PROTECTED;
//    //        Config.CredConfig.CertificateFileProtected = &Config.CertFileProtected;
//    //    }
//    //    else {
//    //        Config.CertFile.CertificateFile = (char*)Cert;
//    //        Config.CertFile.PrivateKeyFile = (char*)KeyFile;
//    //        Config.CredConfig.Type = QUIC_CREDENTIAL_TYPE_CERTIFICATE_FILE;
//    //        Config.CredConfig.CertificateFile = &Config.CertFile;
//    //    }
//
//    //}
//    //else {
//    //    printf("Must specify ['-cert_hash'] or ['cert_file' and 'key_file' (and optionally 'password')]!\n");
//    //    return FALSE;
//    //}
//
//    //
//    // Allocate/initialize the configuration object, with the configured ALPN
//    // and settings.
//    //
//    //auto _status = QUIC_STATUS_SUCCESS;
//    //p_ctx->api->ConfigurationOpen(Registration, &Alpn, 1, &Settings, sizeof(Settings), NULL, &Configuration)
//    //if (_status != S_OK)
//    //{
//    //    printf("ConfigurationOpen failed, 0x%x!\n", Status);
//    //    return FALSE;
//    //}
//
//    //
//    // Loads the TLS credential part of the configuration.
//    //
//    //if (QUIC_FAILED(Status = MsQuic->ConfigurationLoadCredential(Configuration, &Config.CredConfig))) {
//    //    printf("ConfigurationLoadCredential failed, 0x%x!\n", Status);
//    //    return FALSE;
//    //}
//
//    return W_RESULT::SUCCESS;
//}
//
//_IRQL_requires_max_(DISPATCH_LEVEL)
//_Function_class_(QUIC_CONNECTION_CALLBACK)
//QUIC_STATUS QUIC_API s_server_connection_callback(
//    _In_ HQUIC p_connection,
//    _In_opt_ void* p_context,
//    _Inout_ QUIC_CONNECTION_EVENT* p_event) noexcept
//{
//    auto _ctx = gsl::narrow_cast<w_quic::q_ctx*>(p_context);
//    if (_ctx == nullptr || p_event == nullptr)
//    {
//        return QUIC_STATUS_INVALID_PARAMETER;
//    }
//
//    switch (p_event->Type)
//    {
//    case QUIC_CONNECTION_EVENT_CONNECTED:
//    {
//        // The handshake has completed for the connection.
//
//        //printf("[conn][%p] Connected\n", p_connection);
//        _ctx->api->ConnectionSendResumptionTicket(
//            p_connection,
//            QUIC_SEND_RESUMPTION_FLAG_NONE,
//            0,
//            nullptr);
//        break;
//    }
//    case QUIC_CONNECTION_EVENT_SHUTDOWN_INITIATED_BY_TRANSPORT:
//    {
//        // The connection has been shut down by the transport. Generally, this
//        // is the expected way for the connection to shut down with this
//        // protocol, since we let idle timeout kill the connection.
//        if (p_event->SHUTDOWN_INITIATED_BY_TRANSPORT.Status == QUIC_STATUS_CONNECTION_IDLE)
//        {
//            //fmt::print("connection {} successfully shut down on idle.\n", p_connection);
//        }
//        else
//        {
//            /*fmt::print("connection {} shut down by transport, {:x}\n",
//                p_connection,
//                p_event->SHUTDOWN_INITIATED_BY_TRANSPORT.Status);
//                */
//        }
//        break;
//    }
//    case QUIC_CONNECTION_EVENT_SHUTDOWN_INITIATED_BY_PEER:
//    {
//        // The connection was explicitly shut down by the peer.
//        /*fmt::print("connection {} shut down by peer, {}\n",
//            p_connection,
//            (unsigned long long)p_event->SHUTDOWN_INITIATED_BY_PEER.ErrorCode);
//            */
//        break;
//    }
//    case QUIC_CONNECTION_EVENT_SHUTDOWN_COMPLETE:
//    {
//        // The connection has completed the shutdown process and is ready to be
//        // safely cleaned up.
//        //fmt::print("connection {} all done\n", p_connection);
//        _ctx->api->ConnectionClose(p_connection);
//        break;
//    }
//    case QUIC_CONNECTION_EVENT_PEER_STREAM_STARTED:
//    {
//        // The peer has started/created a new stream. The app MUST set the
//        // callback handler before returning.
//        //fmt::print("stream {} peer started\n", p_event->PEER_STREAM_STARTED.Stream);
//        _ctx->api->SetCallbackHandler(
//            p_event->PEER_STREAM_STARTED.Stream,
//            (void*)s_server_stream_callback,
//            nullptr);
//        break;
//    }
//    case QUIC_CONNECTION_EVENT_RESUMED:
//    {
//        // The connection succeeded in doing a TLS resumption of a previous
//        // connection's session.
//        //fmt::print("connection {} connection resumed!\n", p_connection);
//        break;
//    }
//    default:
//        break;
//    }
//    return QUIC_STATUS_SUCCESS;
//}
//
//#endif