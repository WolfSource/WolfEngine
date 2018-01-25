/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : pch.hpp
	Description		 : Pre-Compiled header of pyWolf
	Comment          :
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __PCH_H__
#define __PCH_H__


#include <boost/python.hpp>

#include <tbb/atomic.h>

//from wolf::system namespaces
#include <wolf.h>
#include <w_bounding.h>

#endif //__PCH_H__
