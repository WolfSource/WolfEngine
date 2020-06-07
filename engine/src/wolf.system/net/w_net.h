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

/**
 * w_net_init is called each time the user enters the library. It ensures that
 * the library is initlialized properly, and also deals with checks such as
 * whether the process has forked since last initialization.
*/
W_RESULT w_net_init(void);

/**
 * parses a URL string into a structured form
 * @param pUrlAddress opened file
 * @param pURL structured url
 * @return result code
*/
W_RESULT w_net_url_parse(_In_z_ const char* pUrlAddress, _Inout_ w_url pURL);

/**
 * free a URL resources
 * @param pURL structured url
*/
void w_net_url_free(_Inout_ w_url pURL);

/**
 * create and open a tcp socket
 * @param pEndPoint endpoint address like tcp://127.0.x.1:1234
 * @param pSocketMode tcp socket mode
 * @param pNoDelayOption an option for tcp socket
 * @param pKeepAliveOption an option for tcp socket
 * @param pAsync create dialer or listener in async mode
 * @param pTLS set true if tls is enable
 * @param pTLSServerName server name
 * @param pOwnCert initialize with own certificate
 * @param pCert tls certificate
 * @param pKey tls key
 * @param pSocket a tcp socket created
 * @return result code
*/
W_RESULT w_net_open_tcp_socket(_In_z_ const char* pEndPoint,
                                    _In_ w_socket_mode pSocketMode,
                                    _In_ const bool pNoDelayOption,
                                    _In_ const bool pKeepAliveOption,
                                    _In_ const bool pAsync,
                                    _In_ const bool pTLS,
                                    _In_z_ const char* pTLSServerName,
                                    _In_ const bool pOwnCert,
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
                        _In_ const size_t pMessageLength,
                        _In_ const bool pAsync);

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
 * convert error code to string message
 * @param pErrorCode error code
 * @return string message of error code
*/
const char* w_net_error(_In_ const W_RESULT pErrorCode);

/**
 * w_net_fini is used to terminate the library, freeing certain global resources.
 * This should only be called during at exit or just before releasing dll
*/
void w_net_fini(void);

#ifdef __cplusplus
}
#endif
