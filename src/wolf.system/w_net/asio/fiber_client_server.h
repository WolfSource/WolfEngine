/*
    Project          : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
    Source           : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
    Website          : https://WolfEngine.App
    Name             : fiber_server.h
    Description      : an asio fiber base server wrapper
    Comment          :
*/

#pragma once

#ifdef WOLF_ENABLE_FIBER

#ifdef __cplusplus
extern "C" {
#endif

#include <wolf.h>

    typedef W_RESULT(*w_fiber_server_callback_fn)(_In_ void* /*pSocket*/, _In_ w_buffer /*pReceivedBuffer*/, _In_z_ const char* /*pThreadFiberInfo*/);
    typedef W_RESULT(*w_fiber_client_callback_fn)(_In_ void* /*pSocket*/, _In_z_ const char* /*pThreadFiberInfo*/);

    /**
     * async write with fibers
     * @param pSocket pointer to socket
     * @param pBuffer the send buffer
     * @return result code
    */
    W_RESULT fiber_async_write(_In_ void* pSocket, _In_ w_buffer pBuffer);

    /**
     * async read with fibers
     * @param pSocket pointer to socket
     * @param pBuffer the received buffer
     * @param pReplyLength the length of reply
     * @return result code
    */
    W_RESULT fiber_async_read(
        _In_ void* pSocket,
        _Inout_ w_buffer* pBuffer,
        _Inout_ size_t* pReplyLength);

    /**
     * run a server base on fiber threads
     * @param pIPV4_OR_IPV6 set IPV4 or IPV6 protocols
     *  <PRE>
     *      3 = IPV4 
     *      4 = IPV6
     *  </PRE>
     * @param pPort the port number
     * @param pID the id of fiber server, use this id for stop server via fiber_server_stop
     * @param pOnReceivedCallback fiber callback function 
     * @return result code
    */
    W_RESULT fiber_server_run(
        _In_ const int pIPV4_OR_IPV6, 
        _In_ const uint16_t pPort,
        _In_ int** pID,
        _In_ w_fiber_server_callback_fn pOnReceivedCallback);

    /**
     * stop the fiber server
     * @param pID the id of fiber server
     * @return result code
    */
    W_RESULT fiber_server_stop(_In_ const int pID);

    /**
     * connect to server with fibers based connection
     * @param pIPV4_OR_IPV6 set IPV4 or IPV6 protocols
     *  <PRE>
     *      3 = IPV4
     *      4 = IPV6
     *  </PRE>
     * @param pEndPoint, the endpoint connection
     * @param pPort, the endpoint port
     * @param pNumberOfClients, the number of concurrent clients
     * @param pOnSendReceiveCallback, fiber callback function 
     * @return result code
    */
    W_RESULT fiber_clients_connect(
        _In_ const int pIPV4_OR_IPV6,
        _In_ const char* pEndPoint,
        _In_ const uint16_t pPort,
        _In_ const int pNumberOfClients,
        _In_ w_fiber_client_callback_fn pOnSendReceiveCallback);

#ifdef __cplusplus
}
#endif

#endif
