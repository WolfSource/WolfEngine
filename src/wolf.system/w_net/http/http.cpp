#ifdef WOLF_ENABLE_HTTP1_1_WS

#include "http.h"

#include "helpers/AsyncFileReader.hpp"
#include "helpers/AsyncFileStreamer.hpp"
#include "helpers/Middleware.hpp"

#include <w_io/w_io.h>

void run_http1_1_server(
    _In_ const bool pSSL,
    _In_opt_z_ const char* pCertFilePath,
    _In_opt_z_ const char* pPrivateKeyFilePath,
    _In_opt_z_ const char* pPassPhrase,
    _In_opt_z_ const char* pRoot,
    _In_opt_z_ const char* pURLPath,
    _In_ const int pPort,
    _In_ http_on_listened_fn pOnListened)
{
    std::string _root = "";
    if (pRoot == nullptr || pRoot[0] == '\0')
    {
        w_mem_pool _mem_pool = nullptr;
        
        if (w_mem_pool_init(&_mem_pool) == W_SUCCESS)
        {
            char* _current_exe_dir = nullptr;
            if (w_io_dir_get_current_exe(_mem_pool, &_current_exe_dir) == W_SUCCESS)
            {
                _root = _current_exe_dir;
            }
            w_mem_pool_fini(&_mem_pool);
        }
        else
        {
            return;
        }
    }
    else
    {
        _root = pRoot;
    }

    AsyncFileStreamer _async_file_streamer(_root);

    std::string _url_path = 
        (pURLPath == nullptr || pURLPath[0] == '\0') ? "/*" : pURLPath;

    //server over https or http
    if (pSSL)
    {
        if (!pCertFilePath || !pPrivateKeyFilePath || !pPassPhrase)
        {
            return;
        }
        // HTTPS
        uWS::SSLApp(
            {
                .key_file_name = pPrivateKeyFilePath,
                .cert_file_name = pCertFilePath,
                .passphrase = pPassPhrase,
                .ssl_prefer_low_memory_usage = 1
            }).get(_url_path, [&_async_file_streamer](auto* pRes, auto* pReq)
                {
                    serveFile(pRes, pReq);
                    _async_file_streamer.streamFile(pRes, pReq->getUrl());
                }).listen(pPort, [&](auto* pToken)
                    {
                        if (pToken)
                        {
                            pOnListened((void*)pToken, _root.c_str(), pPort);
                        }
                    }).run();
    }
    else
    {
        // HTTP
        uWS::App().get(_url_path, [&_async_file_streamer](auto* pRes, auto* pReq)
            {
                serveFile(pRes, pReq);
                _async_file_streamer.streamFile(pRes, pReq->getUrl());
            }).listen(pPort, [&](auto* pToken)
                {
                    if (pToken)
                    {
                        pOnListened((void*)pToken, _root.c_str(), pPort);
                    }
                }).run();
    }
}

void stop_http1_1_server(_Inout_ void* pUSocket, _In_ const bool pSSL)
{
    if (pUSocket)
    {
        auto _s = (struct us_listen_socket_t*)pUSocket;
        us_listen_socket_close(pSSL, _s);
    }
}

#endif