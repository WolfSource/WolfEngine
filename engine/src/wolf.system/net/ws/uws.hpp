/*
    Project          : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
    Source           : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
    Website          : https://WolfEngine.App
    Name             : uws.hpp
    Description      : simple,secure & standards websocket based on (https://github.com/uNetworking/uWebSockets)
    Comment          :
*/

#pragma once

#include <functional>
#include "uWebSockets/App.h"

struct per_ws_data
{
    void* d;
};

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
             const int pMaxPayloadLength,
             const int pIdleTimeout,
             const int pMaxBackPressure,
             std::function<void(int)> pOnListened,
             std::function<bool(void**)> pOnOpened,
             std::function<const char*(const char*, int*, void**)> pOnMessage,
             std::function<void(const char*, int, void**)> pOnClosed);
};

