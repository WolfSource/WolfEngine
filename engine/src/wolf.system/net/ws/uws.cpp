#include "uws.hpp"
#include <io/w_io.h>

uws::uws()
{
}

uws::~uws()
{
}

int uws::run(const bool pSSL,
              const char* pCertFilePath,
              const char* pPrivateKeyFilePath,
              const char* pPassPhrase,
              const char* pRoot,
              const int pPort,
              uWS::CompressOptions pCompression,
              const int pMaxPayloadLength,
              const int pIdleTimeout,
              const int pMaxBackPressure,
              std::function<void(int)> pOnListened,
              std::function<bool(void**)> pOnOpened,
              std::function<const char*(const char*, int*, void**)> pOnMessage,
              std::function<void(const char*, int, void**)> pOnClosed)
{
    if(pSSL)
    {
        if (w_io_file_check_is_file(pCertFilePath))
        {
            return W_FAILURE;
        }
        if (w_io_file_check_is_file(pPrivateKeyFilePath))
        {
            return W_FAILURE;
        }

        uWS::SSLApp(
        {
            .key_file_name = pPrivateKeyFilePath,
            .cert_file_name = pCertFilePath,
            .passphrase = pPassPhrase,
            .ssl_prefer_low_memory_usage = 1
        }).ws<per_ws_data>(pRoot,
        {
            /* Settings */
            .compression = pCompression,
            .maxPayloadLength = pMaxPayloadLength,
            .idleTimeout = pIdleTimeout,
            .maxBackpressure = pMaxBackPressure,
            /* Handlers */
            .open = [&](auto* pWS)
            {
                //get per socket data
                void* _ptr = NULL;
                auto _ret = pOnOpened(&_ptr);
                if (_ret)
                {
                    //ok
                    auto _per_socket_data = (per_ws_data*)pWS->getUserData();
                    _per_socket_data->d = _ptr;
                }
                else
                {
                    //close websocket
                    pWS->close();
                }
            },
            .message = [&](auto* pWS, std::string_view pMessage, uWS::OpCode pOpCode)
            {
                auto _per_socket_data = (per_ws_data*)pWS->getUserData();
                auto _res = pOnMessage(pMessage.data(), (int*)&pOpCode, &_per_socket_data->d);
                if (_res == NULL)
                {
                    //close websocket
                    pWS->close();
                }
                else
                {
                    pWS->send(_res, pOpCode, /*compress*/true);
                }
            },
            .drain = [](auto* pWS)
            {
                /* Check ws->getBufferedAmount() here */
            },
            .ping = [](auto* pWS)
            {
                /* Not implemented yet */
            },
            .pong = [](auto* pWS)
            {
                /* Not implemented yet */
            },
            .close = [&](auto* pWS, int pExitCode, std::string_view pMessage)
            {
                auto _per_socket_data = (per_ws_data*)pWS->getUserData();
                pOnClosed(pMessage.data(), pExitCode, &_per_socket_data->d);
            }
        }).listen(pPort, [&](auto* pToken)
        {
            if (pToken)
            {
                pOnListened(pPort);
            }
        }).run();
    }
    else
    {
        uWS::App(
        {
            .passphrase = pPassPhrase
        }).ws<per_ws_data>(pRoot,
        {
            /* Settings */
            .compression = pCompression,
            .maxPayloadLength = pMaxPayloadLength,
            .idleTimeout = pIdleTimeout,
            .maxBackpressure = pMaxBackPressure,
            /* Handlers */
            .open = [&](auto* pWS)
            {
                //get per socket data
                void* _ptr = NULL;
                pOnOpened(&_ptr);

                auto _per_socket_data = (per_ws_data*)pWS->getUserData();
                _per_socket_data->d = _ptr;
            },
            .message = [&](auto* pWS, std::string_view pMessage, uWS::OpCode pOpCode)
            {
                auto _per_socket_data = (per_ws_data*)pWS->getUserData();
                auto _res = pOnMessage(pMessage.data(), (int*)&pOpCode, &_per_socket_data->d);
                if (_res == NULL)
                {
                    //close websocket
                    pWS->close();
                }
                else
                {
                    pWS->send(_res, pOpCode, /*compress*/true);
                }
            },
            .drain = [](auto* pWS)
            {
                /* Check ws->getBufferedAmount() here */
            },
            .ping = [](auto* pWS)
            {
                /* Not implemented yet */
            },
            .pong = [](auto* pWS)
            {
                /* Not implemented yet */
            },
            .close = [&](auto* pWS, int pExitCode, std::string_view pMessage)
            {
                auto _per_socket_data = (per_ws_data*)pWS->getUserData();
                pOnClosed(pMessage.data(), pExitCode, &_per_socket_data->d);
            }
        }).listen(pPort, [&](auto* pToken)
        {
            if (pToken)
            {
                pOnListened(pPort);
            }
        }).run();
    }
    return W_SUCCESS;
}
