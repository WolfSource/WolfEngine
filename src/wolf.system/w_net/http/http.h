/*
    Project          : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
    Source           : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
    Website          : https://WolfEngine.App
    Name             : http.h
    Description      : a C wrapper over a simple,secure & standards uwebsocket http 1.1
                       (https://github.com/uNetworking/uWebSockets)
    Comment          :
*/

#pragma once

#ifdef WOLF_ENABLE_HTTP1_1_WS

#ifdef __cplusplus
extern "C" {
#endif

#include <wolf.h>

    typedef void(*http_on_listened_fn)(void* /*pSocket*/, const char* /*pRoot*/, const int /*pPort*/);

    /**
     * run a http server and block the current thread
     * @param pSSL enable SSL
     * @param pCertFilePath the certificate path for SSL. use nullptr if pSSL is false
     * @param pPrivateKeyFilePath the private key file for SSL. use nullptr if pSSL is false
     * @param pPassPhrase set Pass Pharse
     * @param pRoot the root of serving.
     * @param pURLPath the url path of serving, default is /*
     * @param pPort the websocket port
     * @param pOnListened on listened callback
     * @return result code
    */
    void run_http1_1_server(
        _In_ const bool pSSL,
        _In_opt_z_ const char* pCertFilePath,
        _In_opt_z_ const char* pPrivateKeyFilePath,
        _In_opt_z_ const char* pPassPhrase,
        _In_opt_z_ const char* pRoot,
        _In_opt_z_ const char* pURLPath,
        _In_ const int pPort,
        _In_ http_on_listened_fn pOnListened);

    /**
     * stop http server
     * @param pUSocket a pointer to uSocket object
     * @param pSSL the SSL state
     */
    void stop_http1_1_server(_Inout_ void* pUSocket, _In_ const bool pSSL);

#ifdef __cplusplus
}
#endif

#endif
