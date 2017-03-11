/*
	Project			 : Wolf Engine (http://WolfSource.io). Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : https://github.com/PooyaEimandar/WolfEngine - Please direct any bug to hello@WolfSource.io or tweet @Wolf_Engine on twitter
	Name			 : PCH.h
	Description		 : The pre-compiled header 
	Comment          :
*/

#ifndef __PCH_H__
#define __PCH_H__

#include "w_target_ver.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>
#include <memory>
#include <vector>
#include <tuple>

//#include <boost/asio.hpp>
//extern boost::asio::io_service service;

extern std::vector<std::tuple<HWND, int, int>> sTextBoxesHandles;

#endif