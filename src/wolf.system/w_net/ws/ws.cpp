#ifdef WOLF_ENABLE_HTTP1_1_WS

#include "ws.h"

#include <apr.h>
#include <apr_general.h>
#include <apr_strings.h>
#include <apr_tables.h>
#include <apr_file_io.h>
#include <apr_hash.h>
#include "uws.hpp"

ws ws_init()
{
    try
    {
      return reinterpret_cast<ws>(new uws());
    }
    catch (...)
    {
      return nullptr;
    }
}

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
    _In_ ws_on_closed_fn pOnClosed)
{
    W_RESULT _rt;
    try
    {
        _rt = reinterpret_cast<uws*>(pWS)->run(
            pSSL,
            pCertFilePath,
            pPrivateKeyFilePath,
            pPassPhrase,
            pRoot,
            pPort,
            static_cast<uWS::CompressOptions>(pCompression),
            pMaxPayloadLength,
            pIdleTimeout,
            pMaxBackPressure,
            pOnListened,
            pOnOpened,
            pOnMessage,
            pOnClosed);
    }
    catch (...)
    {
        _rt = W_FAILURE;
    }
    return _rt;
}

void ws_stop(_Inout_ void* pUSocket, _In_ const bool pSSL)
{
    if (pUSocket)
    {
        auto _s = (struct us_listen_socket_t*)pUSocket;
        us_listen_socket_close(pSSL, _s);
    }
}

void ws_free(_Inout_ ws pWS)
{
    if (pWS)
    {
        try
        {
            delete reinterpret_cast<uws*>(pWS);
        }
        catch( ...)
        {
        }
    }
}

#endif