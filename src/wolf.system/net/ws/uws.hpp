/*
    Project          : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
    Source           : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
    Website          : https://WolfEngine.App
    Name             : uws.hpp
    Description      : simple,secure & standards websocket based on (https://github.com/uNetworking/uWebSockets)
    Comment          :
*/

#pragma once

#ifdef WOLF_ENABLE_WEBSOCKET

#include <functional>
#include "uWebSockets/App.h"
#include <wolf.h>

class uws
{
public:
    explicit uws(void);
    ~uws(void);

    int run(const bool pSSL,
        const char* pCertFilePath,
        const char* pPrivateKeyFilePath,
        const char* pPassPhrase,
        const char* pRoot,
        const int pPort,
        uWS::CompressOptions pCompression,
        unsigned int pMaxPayloadLength,
        unsigned int pIdleTimeout,
        unsigned int pMaxBackPressure,
        std::function<void(void*, int)> pOnListened,
        std::function<bool(w_arg*)> pOnOpened,
        std::function<const char* (const char*, size_t, int*, w_arg*)> pOnMessage,
        std::function<void(const char*, size_t, int, w_arg*)> pOnClosed);
};

#endif