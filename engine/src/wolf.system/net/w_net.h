/*
    Project          : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
    Source           : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
    Website          : https://WolfEngine.App
    Name             : w_net.h
    Description      : a light-weight brokerless messaging system based on (https://github.com/nanomsg/nng)
    Comment          :
*/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "wolf.h"
#include "ws/ws.h"
#include <memory/w_array.h>

    //forward declaration
    typedef struct nng_socket* w_socket;
    typedef struct nng_dialer* w_dialer;
    typedef struct nng_listener* w_listener;
    typedef struct nni_plat_udp* w_udp_protocol;
    typedef struct nng_aio* w_aio;
    typedef struct nng_iov* w_iov;
    typedef union nng_sockaddr* w_socket_address;

    //base on nng url
    typedef struct w_url_t
    {
        char* rawurl;   // never NULL
        char* scheme;   // never NULL
        char* userinfo; // will be NULL if not specified
        char* host;     // including colon and port
        char* hostname; // name only, will be "" if not specified
        char* port;     // port, will be "" if not specified
        char* path;     // path, will be "" if not specified
        char* query;    // without '?', will be NULL if not specified
        char* fragment; // without '#', will be NULL if not specified
        char* requri;   // includes query and fragment, "" if not specified
    } w_url_t;
    typedef struct w_url_t* w_url;

    typedef enum
    {
        one_way_pusher,
        one_way_puller,
        two_way_dialer,
        two_way_listener,
        req_rep_dialer,
        req_rep_listener,
        pub_sub_broadcaster,
        pub_sub_subscriber,
        survey_respond_server,
        survey_respond_client,
        bus_node,
        quic_dialer,
        quic_listener
    } w_socket_mode;

    typedef struct w_buffer_t
    {
        uint8_t* data;
        size_t      len;
    } w_buffer_t;
    typedef w_buffer_t* w_buffer;

    typedef struct
    {
        w_socket        s;
        w_dialer        d;
        w_listener      l;
    } w_socket_tcp;

    typedef struct
    {
        w_socket_address    sa;
        w_udp_protocol      u;
        w_aio               a;
        w_iov               i;
    } w_socket_udp;

    typedef enum
    {
        UNSPEC = 0,
        IPV4 = 3,
        IPV6 = 4
    } w_tcp_socket_address_family;

    typedef enum
    {
        //  Requests using GET should only retrieve data.
        HTTP_GET = 0,
        /*
            The HEAD method asks for a response identical to that of a GET request,
         but without the response body.
        */
        HTTP_HEAD,
        /*
            The POST method is used to submit an entity to the specified resource,
            often causing a change in state or side effects on the server.
        */
        HTTP_POST,
        /*  The PUT method replaces all current representations of the target resource with the request payload. */
        HTTP_PUT,
        /*  The DELETE method deletes the specified resource. */
        HTTP_DELETE,
        /*   The CONNECT method establishes a tunnel to the server identified by the target resource. */
        HTTP_CONNECT,
        /*  The OPTIONS method is used to describe the communication options for the target resource. */
        HTTP_OPTIONS,
        /*  The TRACE method performs a message loop-back test along the path to the target resource. */
        HTTP_TRACE,
        /*  The PATCH method is used to apply partial modifications to a resource. */
        HTTP_PATCH
    } w_http_request_type;

    // quic receive callback
    typedef void (*quic_debug_log_callback_fn)(const char* /*pLine*/, void* /*pArgp*/);
    typedef W_RESULT(*quic_stream_callback_fn)(uint8_t* /*pConnectionID*/, uint64_t /*pStreamIndex*/,
        const uint8_t* /*pProtocol*/, const size_t /*pProtocol*/);

    /**
     * w_net_init is called each time the user enters the library. It ensures that
     * the library is initlialized properly, and also deals with checks such as
     * whether the process has forked since last initialization.
     * @return result code
    */
    W_SYSTEM_EXPORT
        W_RESULT w_net_init(void);

    /**
     * parses a URL string into a structured form
     * @param pMemPool The pool to allocate out of
     * @param pUrlAddress url address
     * @param pURL structured url
     * @return result code
    */
    W_SYSTEM_EXPORT
        W_RESULT w_net_url_parse(
            _In_ w_mem_pool pMemPool,
            _In_z_ const char* pUrlAddress,
            _Inout_ w_url* pURL);

    /**
     * encode a URL string
     * @param pMemPool The pool to allocate out of
     * @param pUrlAddress opened url
     * @return result code
    */
    W_SYSTEM_EXPORT
        const char* w_net_url_encoded(
            _Inout_ w_mem_pool pMemPool,
            _In_z_ const char* pUrlAddress);

    /**
     * Resolve a TCP name asynchronously
     * @param pURL the address url
     * @param pPort the port
     * @param pSocketAddressFamily the address family should be one of ipv4 or ipv6
     * @param pBindOrConnect the passive flag indicates that the name will be used for bind(), otherwise the name will be used with connect()
     * @param pSocketAddress socket address information
     * @return result code
    */
    W_SYSTEM_EXPORT
        W_RESULT w_net_resolve(_In_z_ const char* pURL,
            _In_z_ const char* pPort,
            _In_ w_tcp_socket_address_family pSocketAddressFamily,
            _In_ int pBindOrConnect,
            _Inout_ w_socket_address pSocketAddress);

    /**
     * create and open a tcp socket
     * @param pEndPoint endpoint address like tcp://127.0.x.1:1234
     * @param pSocketMode tcp socket mode
     * @param pNoDelayOption an option for tcp socket
     * @param pKeepAliveOption an option for tcp socket
     * @param pAsync create dialer or listener in async mode
     * @param pTLS set true if tls is enable
     * @param pAuthMode authentication mode flags for listener + tls mode
     * <PRE>
     *      NNG_TLS_AUTH_MODE_NONE : No authentication of the TLS peer is performed. This is the default for TLS servers, which most typically do not authenticate their clients.
     *
     *      NNG_TLS_AUTH_MODE_OPTIONAL: If a certificate is presented by the peer, then it is validated. However, if the peer does not present a valid certificate, then the session is allowed to proceed without authentication.
     *
     *      NNG_TLS_AUTH_MODE_REQUIRED: A check is made to ensure that the peer has presented a valid certificate used for the session. If the peer’s certificate is invalid or missing, then the session is refused. This is the default for clients.
     * </PRE>
     * @param pTLSServerName server name
     * @param pOwnCert initialize with own certificate
     * @param pCert tls certificate
     * @param pKey tls private key
     * @param pSocket a tcp socket created
     * @return result code
    */
    W_SYSTEM_EXPORT
        W_RESULT w_net_open_tcp_socket(
            _In_ w_mem_pool pMemPool,
            _In_z_ const char* pEndPoint,
            _In_ w_socket_mode pSocketMode,
            _In_ bool pNoDelayOption,
            _In_ bool pKeepAliveOption,
            _In_ bool pAsync,
            _In_ bool pTLS,
            _In_ int pAuthMode,
            _In_opt_z_ const char* pTLSServerName,
            _In_ bool pOwnCert,
            _In_opt_z_ const char* pCert,
            _In_opt_z_ const char* pKey,
            _Inout_ w_socket_tcp* pSocket);


    /**
     * close a tcp socket
     * @param pSocket tcp socket
     * @return result code
    */
    W_SYSTEM_EXPORT
        W_RESULT w_net_close_tcp_socket(_Inout_ w_socket_tcp* pSocket);

    /**
     * create and open a tcp socket
     * @param pMemPool The pool to allocate out of
     * @param pEndPoint endpoint address like udp://127.0.x.1:1234
     * @param pSocket a udp socket created
     * @return result code
    */
    W_SYSTEM_EXPORT
        W_RESULT w_net_open_udp_socket(
            _Inout_ w_mem_pool pMemPool,
            _In_z_ const char* pEndPoint,
            _Inout_ w_socket_udp* pSocket);

    /**
     * close a udp socket
     * @param pSocket a udp socket
    */
    W_SYSTEM_EXPORT
        void w_net_close_udp_socket(_Inout_ w_socket_udp* pSocket);

    /**
     * create a server based on QUIC protocol
     * @param pAddress , host address
     * @param pPort , host port
     * @param pSocketMode , the socket mode. only the following enums will be accepted
       <PRE>
            quic_dialer
            quic_listener
       </PRE>
     * @param pCertFilePath , path of certificate chain file. Will be used when pSocketMode set to quic_listener
     * @param pPrivateKeyFilePath , path of private key file. Will be used when pSocketMode set to quic_listener
     * @param pQuicDebugLogCallback , quic debugger call back. set NULL if you don't want to use debugger
     * @return result
    */
    W_SYSTEM_EXPORT
        W_RESULT w_net_open_quic_socket(
            _In_z_  const char* pAddress,
            _In_        int pPort,
            _In_        w_socket_mode pSocketMode,
            _In_opt_z_  const char* pCertFilePath,
            _In_opt_z_  const char* pPrivateKeyFilePath,
            _In_opt_    quic_debug_log_callback_fn pQuicDebugLogCallback,
            _In_opt_    quic_stream_callback_fn pQuicReceivingStreamCallback,
            _In_opt_    quic_stream_callback_fn pQuicSendingStreamCallback);

    /**
     * Close quic socket
     * @return result
    */
    W_SYSTEM_EXPORT
        W_RESULT w_net_close_quic_socket();

    /**
     * Writes data to a stream of quic protocol
     * @param pConnectionID , connection id which is a uint8_t[16]
     * @param pStreamIndex , the current index of quic stream
     * @param pBuffer the buffer which is going to send
     * @param pFinish send finish as the end of stream
     * @return number of sent bytes
    */
    W_SYSTEM_EXPORT
        size_t w_net_send_msg_quic(
            _In_ uint8_t* pConnectionID,
            _In_ uint64_t pStreamIndex,
            _In_ w_buffer pBuffer,
            _In_ bool pFinish);

    /**
     * Reads contiguous data from a stream.
     * @param pConnectionID , connection id which is a uint8_t[16]
     * @param pStreamIndex , the current index of quic stream
     * @param pReceiveBuffer , pointer to received buffer
     * @param pIsStreamFinished true means stream was finished
     * @return number of received bytes
    */
    W_SYSTEM_EXPORT
        size_t w_net_receive_msg_quic(
            _In_ uint8_t* pConnectionID,
            _In_ uint64_t pStreamIndex,
            _Inout_ w_buffer pReceiveBuffer,
            _Inout_ bool* pIsStreamFinished);

    /**
     * send a message via tcp socket
     * @param pSocket a tcp socket
     * @param pBuffer buffer
     * @param pAsync asynchronous mode
     * @return number of sent bytes
    */
    W_SYSTEM_EXPORT
        int w_net_send_msg_tcp(
            _Inout_ w_socket_tcp* pSocket,
            _In_ w_buffer pBuffer,
            _In_ bool pAsync);

    /**
     * receive a message via tcp socket
     * @param pSocket a tcp socket
     * @param pBuffer message buffer
     * @return number of received bytes
    */
    W_SYSTEM_EXPORT
        int w_net_receive_msg_tcp(
            _Inout_ w_socket_tcp* pSocket,
            _Inout_ w_buffer pBuffer);

    /**
     * send data via udp socket
     * @param pSocket udp socket
     * @param pMessage buffer
     * @param pMessageLength length of the send buffer
     * @return result code
    */
    W_SYSTEM_EXPORT
        W_RESULT w_net_send_msg_udp(
            _Inout_ w_socket_udp* pSocket,
            _In_z_ char* pMessage,
            _In_ size_t pMessageLength);

    /**
     * read data from udp socket
     * @param pSocket udp socket
     * @param pMessage buffer
     * @param pMessageLength length of the recieved buffer
     * @return result code
    */
    W_SYSTEM_EXPORT
        W_RESULT w_net_receive_msg_udp(
            _Inout_ w_socket_udp* pSocket,
            _In_z_ char* pMessage,
            _In_z_ size_t* pMessageLength);

    /**
     * free message
     * @param pMsg the message buffer
     * @return result code
     */
    W_SYSTEM_EXPORT
        W_RESULT w_net_free_msg(_Inout_ w_buffer pMsg);

    /**
     * run a websocket server and block the current thread
     * @param pSSL enable SSL(based on WolfSSL)
     * @param pCertFilePath the certificate path for SSL. use nullptr if pSSL is false
     * @param pPrivateKeyFilePath the private key file for SSL. use nullptr if pSSL is false
     * @param pPassPhrase set Pass Pharse
     * @param pRoot the root of serving. the default value is /*
     * @param pPort the websocket port
     * @param pCompression type of compression
     *  <PRE>
     *
     *      0 = DISABLED : Compression disabled
     *
     *      1 = SHARED_COMPRESSOR : We compress using a shared non-sliding window. No added memory usage, worse compression.
     *
            2 = DEDICATED_COMPRESSOR : We compress using a dedicated sliding window. Major memory usage added, better compression of similarly repeated messages.

     *      2 | 4 = DEDICATED_COMPRESSOR_3KB
     *      2 | 8 = DEDICATED_COMPRESSOR_4KB
     *      2 | 16 = DEDICATED_COMPRESSOR_8KB
     *      2 | 32 = DEDICATED_COMPRESSOR_16KB
     *      2 | 64 = DEDICATED_COMPRESSOR_32KB
     *      2 | 128 = DEDICATED_COMPRESSOR_64KB
     *      2 | 256 = DEDICATED_COMPRESSOR_128KB
     *      2 | 512 = DEDICATED_COMPRESSOR_256KB
     *  </PRE>
     * @param pMaxPayloadLength the max payload length, default value is (16 * 1024)
     * @param pIdleTimeout Maximum amount of seconds that may pass without sending or getting a message, default value is (120)
     * @param pMaxBackPressure maximum back pressure, default value is (1 * 1024 * 1024)
     * @param pOnListened on listened callback
     * @param pOnOpened on opened callback
     * @param pOnMessage on message callback
     * @param pOnClosed on closed callback
     * @return result code
    */
    W_SYSTEM_EXPORT
        W_RESULT w_net_run_websocket_server(_In_ bool pSSL,
            _In_z_ const char* pCertFilePath,
            _In_z_ const char* pPrivateKeyFilePath,
            _In_z_ const char* pPassPhrase,
            _In_z_ const char* pRoot,
            _In_ int pPort,
            _In_ int pCompression,
            _In_ int pMaxPayloadLength,
            _In_ int pIdleTimeout,
            _In_ int pMaxBackPressure,
            _In_ ws_on_listened_fn pOnListened,
            _In_ ws_on_opened_fn pOnOpened,
            _In_ ws_on_message_fn pOnMessage,
            _In_ ws_on_closed_fn pOnClosed);

    /**
     * send a http request
     * @param pMemPool The pool to allocate out of
     * @param pURL url
     * @param pHttpRequestType http request type
     * @param pHttpHeaders array of constant strings which contains http headers(e.g "Accept: application/json")
     * @param pMessage message (used for example for POST)
     * @param pMessageLenght message lenght
     * @note The following three parameters will be used for aborting request. The process well be aborted if the transferred bytes (pLowSpeedLimit) are less than specific seconds (pLowSpeedTimeInSec)
     * @param pLowSpeedLimit set the acceptable bytes for continuing the job.
     * @param pLowSpeedTimeInSec set the acceptable bytes per seconds for continuing the job
     * @param pTimeOutInSecs timeout in seconds
     * @param pResponseCode the response http code
     * @param pResponseMessage the response message
     * @param pResponseMessageLength the response message size
     * @return curl code
    */
    W_SYSTEM_EXPORT
        W_RESULT w_net_send_http_request(
            _Inout_    w_mem_pool pMemPool,
            _In_z_     const char* pURL,
            _In_       w_http_request_type pHttpRequestType,
            _In_       w_array pHttpHeaders,
            _In_z_     const char* pMessage,
            _In_       size_t pMessageLenght,
            _In_       size_t pLowSpeedLimit,
            _In_       size_t pLowSpeedTimeInSec,
            _In_       float pTimeOutInSecs,
            _Inout_    long* pResponseCode,
            _Inout_z_  char** pResponseMessage,
            _Inout_    size_t* pResponseMessageLength);


    /**
     * convert error code to string message
     * @param pErrorCode error code
     * @return string message of error code
    */
    W_SYSTEM_EXPORT
        const char* w_net_error(_In_ W_RESULT pErrorCode);

    /**
     * w_net_fini is used to terminate the library, freeing certain global resources.
     * This should only be called during at exit or just before releasing dll
    */
    W_SYSTEM_EXPORT
        void w_net_fini(void);

#ifdef __cplusplus
}
#endif
