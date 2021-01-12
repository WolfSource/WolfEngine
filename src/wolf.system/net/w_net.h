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
#include <concurrency/w_thread.h>

#ifdef WOLF_ENABLE_ACTIVEMQ
#include "amq/w_amq.h"
#endif

    typedef struct w_buffer_t
    {
        uint8_t* data;
        size_t      len;
    } w_buffer_t;
    typedef w_buffer_t* w_buffer;

    typedef struct apr_socket_t* w_socket;
    typedef struct apr_sockaddr_t* w_socket_address;

    typedef struct w_socket_options
    {
        int64_t timeout_microseconds;
        bool non_blocking;
        bool no_delay;
        bool keep_alive;
    } w_socket_options;

    typedef enum w_socket_protocol
    {
        W_SOCKET_PROTOCOL_QUIC_DIALER = 0,
        W_SOCKET_PROTOCOL_QUIC_LISTENER = 1,
        W_SOCKET_PROTOCOL_TCP_DIALER = 6,
        W_SOCKET_PROTOCOL_TCP_LISTENER = W_SOCKET_PROTOCOL_TCP_DIALER + 1,
        W_SOCKET_PROTOCOL_UDP_DIALER = 17,
        W_SOCKET_PROTOCOL_UDP_LISTENER = W_SOCKET_PROTOCOL_UDP_DIALER + 1,
        W_SOCKET_PROTOCOL_SCTP_DIALER = 132,
        W_SOCKET_PROTOCOL_SCTP_LISTENER = W_SOCKET_PROTOCOL_SCTP_DIALER + 1,
    } w_socket_protocol;

    typedef enum w_socket_type
    {
        W_SOCKET_TYPE_STREAM = 1, /* stream socket */
        W_SOCKET_TYPE_DGRAM, /* datagram socket */
        W_SOCKET_TYPE_RAW, /* raw-protocol interface */
        W_SOCKET_TYPE_RDM, /* reliably-delivered message */
        W_SOCKET_TYPE_SEQPACKET /* sequenced packet stream */
    } w_socket_type;

    typedef enum w_socket_family
    {
        W_SOCKET_FAMILY_UNSPEC = 0,
        W_SOCKET_FAMILY_IPV4 = 3,
        W_SOCKET_FAMILY_IPV6 = 4,
    } w_socket_family;

    typedef enum w_socket_connection_mode
    {
        ONE_WAY_PUSHER,
        ONE_WAY_PULLER,
        TWO_WAY_DIALER,
        TWO_WAY_LISTENER,
        REQ_REP_DIALER,
        REQ_REP_LISTENER,
        PUB_SUB_BROADCASTER,
        PUB_SUB_SUBSCRIBER,
        SURVEY_RESPOND_SERVER,
        SURVEY_RESPOND_CLIENT,
        BUS_NODE
    } w_socket_connection_mode;

    typedef enum w_http_request_type
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

    //W_SYSTEM_EXPORT
    //    W_RESULT w_net_get_ip_from_hostname(
    //        _In_z_ const char* pHostName,
    //        _Inout_ w_socket_address pSocketAddress);

    //W_SYSTEM_EXPORT
    //    W_RESULT w_net_get_hostname_from_ip(
    //        _Inout_ w_socket_address pSocketAddress,
    //        _Inout_z_ char** pHostName);

    /**
     * create and open a socket
     * @param pMemPool The pool for the socket associated storage
     * @param pEndPoint endpoint address like tcp://0.0.0.0 or tcp://192.168.17.17
     * @param pPort the porn number
     * @param pProtocol protocol The protocol of the socket(e.g., W_SOCKET_PROTOCOL_TCP)
     * @param pType the type pf the socket (e.g., W_SOCKET_TYPE_STREAM)
     * @param pFamily family The address family of the socket (e.g., W_SOCKET_FAMILY_INET)
     * @param pOptions socket options
     * @param pSocket The new socket that has been set up.
     * @return result code
    */
    W_SYSTEM_EXPORT
        W_RESULT w_net_socket_open(
            _In_ w_mem_pool pMemPool,
            _In_opt_z_ const char* pHostName,
            _In_ int pPort,
            _In_ w_socket_protocol pProtocol,
            _In_ w_socket_type pType,
            _In_ w_socket_family pFamily,
            _In_opt_ w_socket_options* pOptions,
            _Inout_ w_socket* pSocket);

    /**
     * close a socket
     * @param pSocket socket
     * @return result code
    */
    W_SYSTEM_EXPORT
        W_RESULT w_net_socket_close(_Inout_ w_socket* pSocket);

    /**
    * accept an incoming new connection
    * @param pMemPool The pool for the socket associated storage
    * @param pSocket the pointer to source socket
    * @param pOptions the socket options
    * @param pAsync create thread for each new accepted connection
    * @param pOnAcceptCallback accepted socket callback, get the new socket from arg
    * @return result code
    */
    W_SYSTEM_EXPORT
        W_RESULT w_net_socket_accept(
            _Inout_ w_mem_pool pMemPool,
            _In_ w_socket pSocket,
            _In_ w_socket_options* pOptions,
            _In_ bool pAsync,
            _In_ w_thread_job pOnAcceptCallback);

    /**
    * send a message via socket
    * @param pSocket the pointer to w_socket_t object
    * @param pBuffer buffer
    * @return result code
    */
    W_SYSTEM_EXPORT
        W_RESULT w_net_socket_send(
            _Inout_ w_socket pSocket,
            _In_ w_buffer pBuffer);

    /**
     * receive a message via socket
     * @param pSocket the pointer to w_socket_t object
     * @param pBuffer message buffer
     * @return result code
    */
    W_SYSTEM_EXPORT
        W_RESULT w_net_socket_receive(
            _Inout_ w_socket pSocket,
            _Inout_ w_buffer pBuffer);

#ifdef WOLF_ENABLE_WEBSOCKET

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
     *      0 = DISABLED : Compression disabled
     *      1 = SHARED_COMPRESSOR : We compress using a shared non-sliding window. No added memory usage, worse compression.
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
        W_RESULT w_net_run_ws_server(_In_ bool pSSL,
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
    * run a websocket server and block the current thread
    * @param pSocket pointer to socket object
    * @param pSSL is SSL socket
   */
    W_SYSTEM_EXPORT
        void w_net_ws_close(_Inout_ void* pSocket, _In_ const bool pSSL);

#endif

#ifdef WOLF_ENABLE_QUIC

    /**
     * create a server based on QUIC protocol
     * @param pAddress , host address
     * @param pPort , host port
     * @param pProtocol , the socket protocol. only the following enums will be accepted
       <PRE>
            W_SOCKET_PROTOCOL_QUIC_DIALER
            W_SOCKET_PROTOCOL_QUIC_LISTENER
       </PRE>
     * @param pCertFilePath , path of certificate chain file. Will be used when pSocketMode set to quic_listener
     * @param pPrivateKeyFilePath , path of private key file. Will be used when pSocketMode set to quic_listener
     * @param pQuicDebugLogCallback , quic debugger call back. set NULL if you don't want to use debugger
     * @return result
    */
    W_SYSTEM_EXPORT
        W_RESULT w_net_quic_open(
            _In_z_  const char* pAddress,
            _In_        int pPort,
            _In_        w_socket_protocol pProtocol,
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
        W_RESULT w_net_quic_close();

    /**
     * Writes data to a stream of quic protocol
     * @param pConnectionID , connection id which is a uint8_t[16]
     * @param pStreamIndex , the current index of quic stream
     * @param pBuffer the buffer which is going to send
     * @param pFinish send finish as the end of stream
     * @return number of sent bytes
    */
    W_SYSTEM_EXPORT
        size_t w_net_quic_send(
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
        size_t w_net_quic_receive(
            _In_ uint8_t* pConnectionID,
            _In_ uint64_t pStreamIndex,
            _Inout_ w_buffer pReceiveBuffer,
            _Inout_ bool* pIsStreamFinished);

#endif

#ifdef WOLF_ENABLE_CURL

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
        W_RESULT w_net_http_send(
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
     * convert curl error code to string message
     * @param pErrorCode error code
     * @return string message of error code
    */
    W_SYSTEM_EXPORT
        const char* w_net_curl_get_last_error(_In_ W_RESULT pErrorCode);

#endif

#ifdef WOLF_ENABLE_ACTIVEMQ

    /**
     * initialize activeMQ library. Call it only once before using other activeMQ functions.
     * @param pAMQObject The activeMQ object
     * @return result
    */
    W_SYSTEM_EXPORT
        void	w_net_amq_init();

    /**
     * open activemq producer
     * @param pMemPool The pool to allocate out of
     * @param pAMQObject The activeMQ object
     * @param pBrokerURI The broker uri
     * @param pUsername The username 
     * @param pPassword The password 
     * @param pQueueOrTopicName The name of Queue or Topic
     * @param pConnectionType open QUEUE or TOPIC
     * @param pDeliveryMode enable PERSISTENT or NON_PERSISTENT message mode
     * @param pSessionTransacted enable session transacted
     * @return result
    */
    W_SYSTEM_EXPORT
    W_RESULT	w_net_amq_producer_open(
        _Inout_ w_mem_pool pMemPool,
        _Inout_ w_amq_object* pAMQObject,
        _In_z_ const char* pBrokerURI,
        _In_z_ const char* pUsername,
        _In_z_ const char* pPassword,
        _In_z_ const char* pQueueOrTopicName,
        _In_ amq_connection_type pConnectionType,
        _In_ amq_delivery_mode pDeliveryMode,
        _In_ bool pSessionTransacted);

    /**
     * open activemq consumer
     * @param pMemPool The pool to allocate out of
     * @param pAMQObject The activeMQ object
     * @param pBrokerURI The broker uri
     * @param pUsername The username
     * @param pUsername The password
     * @param pQueueOrTopicName The name of Queue or Topic
     * @param pOnMessageCallBack the onMessage callback
     * @param pConnectionType the connection type which is QUEUE or TOPIC
     * @param pSessionTransacted enable session transacted
     * @return result
    */
    W_SYSTEM_EXPORT
    W_RESULT	w_net_amq_consumer_open(
        _Inout_ w_mem_pool pMemPool,
        _Inout_ w_amq_object* pAMQObject,
        _In_z_  const char* pBrokerURI,
        _In_z_  const char* pUsername,
        _In_z_  const char* pPassword,
        _In_z_  const char* pQueueOrTopicName,
        _In_ w_amq_consumer_callback_fn pOnMessageCallBack,
        _In_ amq_connection_type pConnectionType,
        _In_ bool pSessionTransacted);

    /**
     * run activemq producer
     * @param pAMQObject The activeMQ object
     * @return result
    */
    W_SYSTEM_EXPORT
    W_RESULT	w_net_amq_producer_run(
        _In_ w_amq_object pAMQObject);

    /**
     * run activemq consumer
     * @param pAMQObject The activeMQ object
     * @return result
    */
    W_SYSTEM_EXPORT
    W_RESULT	w_net_amq_consumer_run(
        _In_ w_amq_object pAMQObject);

    /**
     * send message to via activeMQ producer
     * @param pAMQObject The activeMQ object
     * @param pMessage The message
     * @param pPriority The priority of message
     * @return result
    */
    W_SYSTEM_EXPORT
    W_RESULT	w_net_amq_producer_send_message(
        _In_ w_amq_object pAMQObject,
        _In_z_  const char* pMessage,
        _In_  int pPriority);

    /**
     * close activemq producer
     * @param pAMQObject The activeMQ object
     * @return result
    */
    W_SYSTEM_EXPORT
    W_RESULT	w_net_amq_producer_close(
        _Inout_ w_amq_object pAMQObject);

    /**
     * close activemq consumer
     * @param pAMQObject The activeMQ object
     * @return result
    */
    W_SYSTEM_EXPORT
    W_RESULT	w_net_amq_consumer_close(
        _Inout_ w_amq_object pAMQObject);

    /**
     * finitialize activemq
     * @param pAMQObject The activeMQ object
     * @return result
    */
    W_SYSTEM_EXPORT
    void	w_net_amq_fini();

#endif

#ifdef __cplusplus
}
#endif
