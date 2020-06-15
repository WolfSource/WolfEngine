#include "uws.hpp"

uws::uws()
{
}

uws::~uws()
{
}

void uws::run(const bool pSSL,
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
              std::function<void(void)> pOnOpened,
              std::function<const char*(const char*, int*)> pOnMessage,
              std::function<void(const char*, int)> pOnClosed)
{
    if(pSSL)
    {
        uWS::SSLApp(
        {
            .key_file_name = pPrivateKeyFilePath,
            .cert_file_name = pCertFilePath,
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
                pOnOpened();
            },
            .message = [&](auto* pWS, std::string_view pMessage, uWS::OpCode pOpCode)
            {
                auto _res = pOnMessage(pMessage.data(), (int*)&pOpCode);
                if (_res)
                {
                    pWS->send(_res, pOpCode);
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
            .close = [&](auto* pWS, int pCode, std::string_view pMessage)
            {
                pOnClosed(pMessage.data(), pCode);
                /* You may access ws->getUserData() here */
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
                pOnOpened();
            },
            .message = [&](auto* pWS, std::string_view pMessage, uWS::OpCode pOpCode)
            {
                auto _res = pOnMessage(pMessage.data(), (int*)&pOpCode);
                if (_res)
                {
                    pWS->send(_res, pOpCode);
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
            .close = [&](auto* pWS, int pCode, std::string_view pMessage)
            {
                pOnClosed(pMessage.data(), pCode);
            }
        }).listen(pPort, [&](auto* pToken)
        {
            if (pToken)
            {
                pOnListened(pPort);
            }
        }).run();
    }
}
