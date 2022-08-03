/*
    Project: Wolf Engine. Copyright 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#pragma once

#include <string>

namespace wolf::stream::webRTC
{
    struct w_ice_server
    {
        std::string url;
        std::string user;
        std::string pass;
    };
}