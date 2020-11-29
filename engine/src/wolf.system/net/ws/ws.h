/*
    Project          : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
    Source           : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
    Website          : https://WolfEngine.App
    Name             : cws.h
    Description      : a C wrapper over a simple,secure & standards websocket
                       namely uWebSockets (https://github.com/uNetworking/uWebSockets)
    Comment          :
*/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <wolf.h>

    //forward declaration
    struct cws;
    typedef struct cws* ws;

    typedef void(*ws_on_listened_fn)(void*/*pSocket*/, int /*pPort*/);
    typedef bool(*ws_on_opened_fn)(w_arg* /*pUserData*/);
    typedef const char* (*ws_on_message_fn)(const char* /*pMessage*/, size_t /*pMessageLenght*/, int* /*pOpCode*/, w_arg* /*pUserData*/);
    typedef void(*ws_on_closed_fn)(const char* /*pMessage*/, size_t /*pMessageLenght*/, int /*pCloseCode*/, w_arg* /*pUserData*/);

    /**
     * create a websocket object
     * @return pointer to websocket object
    */
    ws ws_init(void);

    /**
     * run a websocket server and block the current thread
     * @param pWS a pointer to websocket object
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
    W_RESULT ws_run(_In_ ws pWS,
        _In_ const bool pSSL,
        _In_z_ const char* pCertFilePath,
        _In_z_ const char* pPrivateKeyFilePath,
        _In_z_ const char* pPassPhrase,
        _In_z_ const char* pRoot,
        _In_ const int pPort,
        _In_ const int pCompression,
        _In_ const int pMaxPayloadLength,
        _In_ const int pIdleTimeout,
        _In_ const int pMaxBackPressure,
        _In_ ws_on_listened_fn pOnListened,
        _In_ ws_on_opened_fn pOnOpened,
        _In_ ws_on_message_fn pOnMessage,
        _In_ ws_on_closed_fn pOnClosed);

    /**
     * stop websocket object
     */
    void ws_stop(_Inout_ void* pSocket, _In_ const bool pSSL);

    /**
     * free websocket object
    */
    void ws_free(_Inout_ ws pWS);

#ifdef __cplusplus
}
#endif
