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

//forward declaration
typedef nng_url* w_url;
typedef enum {
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
    bus_node
} w_socket_mode;

typedef nng_sockaddr* w_socket_address;

typedef struct
{
    nng_socket      s;
    nng_dialer      d;
    nng_listener    l;
} w_socket_tcp;

typedef struct
{
    nng_sockaddr    s;
    nni_plat_udp*   u;
    nng_aio*        a;
    nng_iov         i;
} w_socket_udp;

typedef enum
{
    UNSPEC   = 0,
    IPV4     = 3,
    IPV6     = 4
} w_tcp_socket_address_family;

/**
 * w_net_init is called each time the user enters the library. It ensures that
 * the library is initlialized properly, and also deals with checks such as
 * whether the process has forked since last initialization.
*/
W_RESULT w_net_init(void);

/**
 * parses a URL string into a structured form
 * @param pUrlAddress opened url
 * @param pURL structured url
 * @return result code
*/
W_RESULT w_net_url_parse(_In_z_ const char* pUrlAddress, _Inout_ w_url pURL);

/**
 * encode a URL string
 * @param pUrlAddress opened url
 * @return result code
*/
const char* w_net_url_encoded(_In_z_ const char* pUrlAddress);

/**
 * free a URL resources
 * @param pURL structured url
*/
void w_net_url_free(_Inout_ w_url pURL);

/**
 * Resolve a TCP name asynchronously
 * @param pURL the address url
 * @param pPort the port
 * @param pSocketAddressFamily the address family should be one of ipv4 or ipv6
 * @param pBindOrConnect the passive flag indicates that the name will be used for bind(), otherwise the name will be used with connect()
 * @param pSocketAddress socket address information
 * @return result code
*/
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
W_RESULT w_net_open_tcp_socket(_In_z_ const char* pEndPoint,
                                    _In_ w_socket_mode pSocketMode,
                                    _In_ bool pNoDelayOption,
                                    _In_ bool pKeepAliveOption,
                                    _In_ bool pAsync,
                                    _In_ bool pTLS,
                                    _In_ int pAuthMode,
                                    _In_z_ const char* pTLSServerName,
                                    _In_ bool pOwnCert,
                                    _In_z_ const char* pCert,
                                    _In_z_ const char* pKey,
                                    _Inout_ w_socket_tcp* pSocket);

/**
 * close a tcp socket
 * @param pSocket tcp socket
 * @return result code
*/
W_RESULT w_net_close_tcp_socket(_Inout_ w_socket_tcp* pSocket);

/**
 * create and open a tcp socket
 * @param pEndPoint endpoint address like udp://127.0.x.1:1234
 * @param pSocket a udp socket created
 * @return result code
*/
W_RESULT w_net_open_udp_socket(_In_z_ const char* pEndPoint,
                               _Inout_ w_socket_udp* pSocket);

/**
 * close a udp socket
 * @param pSocket a udp socket
*/
void w_net_close_udp_socket(_Inout_ w_socket_udp* pSocket);

/**
 * send a message via tcp socket
 * @param pSocket a tcp socket
 * @param pMessage message buffer
 * @param pMessageLength length of message
 * @param pAsync asynchronous mode
 * @return result code
*/
W_RESULT w_net_send_msg(_Inout_ w_socket_tcp* pSocket,
                        _In_z_ char* pMessage,
                        _In_ size_t pMessageLength,
                        _In_ bool pAsync);

/**
 * receive a message via tcp socket
 * @param pSocket a tcp socket
 * @param pMessage message buffer
 * @param pMessageLength length of message
 * @return result code
*/
W_RESULT w_net_receive_msg(_Inout_ w_socket_tcp* pSocket,
                           _Inout_ char* pMessage,
                           _Inout_ size_t* pMessageLength);

/**
 * send data via udp socket
 * @param pSocket udp socket
 * @param pMessage buffer
 * @param pMessageLength length of the send buffer
 * @return result code
*/
W_RESULT w_net_send_msg_udp(_Inout_ w_socket_udp* pSocket,
                            _In_z_ char* pMessage,
                            _In_z_ size_t pMessageLength);

/**
 * read data from udp socket
 * @param pSocket udp socket
 * @param pMessage buffer
 * @param pMessageLength length of the recieved buffer
 * @return result code
*/
W_RESULT w_net_receive_msg_udp(_Inout_ w_socket_udp* pSocket,
                               _In_z_ char* pMessage,
                               _In_z_ size_t* pMessageLength);
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
 * @param pURL url
 * @note The following three parameters will be used for aborting request. The process well be aborted if the transferred bytes (pLowSpeedLimit) are less than specific seconds (pLowSpeedTimeInSec)
 * @param pLowSpeedLimit set the acceptable bytes for continuing the job.
 * @param pLowSpeedTimeInSec set the acceptable bytes per seconds for continuing the job
 * @param pTimeOutInSecs timeout in seconds
 * @param pResponseCode the response http code
 * @param pResponseMessage the response message
 * @param pResponseMessageLength the response message size
 * @return result code
*/
W_RESULT w_net_send_http_request(_In_z_     const char* pURL,
                                 _In_       size_t pLowSpeedLimit,
                                 _In_       size_t pLowSpeedTimeInSec,
                                 _In_       float pTimeOutInSecs,
                                 _Inout_    long* pResponseCode,
                                 _Inout_z_  char** pResponseMessage,
                                 _Inout_    size_t* pResponseMessageLength);

//W_RESULT w_net_send_http_post(_In_z_ const std::string& pURL,
//                              _In_z_ const std::string& pMessage,
//                              _In_ const size_t& pMessageLenght,
//                              _Inout_ std::string& pResult,
//                              _In_ w_point& pAbortIfSlowerThanNumberOfBytesInSeconds,
//                              _In_ const uint32_t& pConnectionTimeOutInMilliSeconds,
//                              _In_z_ std::initializer_list<std::string> pHeaders);

/**
 * convert error code to string message
 * @param pErrorCode error code
 * @return string message of error code
*/
const char* w_net_error(_In_ W_RESULT pErrorCode);

/**
 * w_net_fini is used to terminate the library, freeing certain global resources.
 * This should only be called during at exit or just before releasing dll
*/
void w_net_fini(void);

#ifdef __cplusplus
}
#endif
